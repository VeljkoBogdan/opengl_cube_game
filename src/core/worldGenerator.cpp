#include "worldGenerator.h"

#include <algorithm>
#include <cmath>

namespace {
using Noise = OpenSimplexNoise::Noise;

// Block ids
constexpr int kAir = 0;
constexpr int kGrass = 1;
constexpr int kDirt = 2;
constexpr int kStone = 3;

// Terrain
constexpr double kBaseHeight = 64.0; // average ground level
constexpr double kHeightAmplitude = 24.0; // how far big hills/valleys move the ground
constexpr double kHeightFreq = 0.004; // size of those hills (lower = wider)
constexpr double kTerrainFreq = 0.02; // size of 3D features (cliffs, arches, overhangs)
constexpr double kTerrainStrength = 1.0; // how strongly 3D noise fights the height bias
constexpr double kTerrainSlope = 0.04;  // density gain per block of depth
// Highest block terrain can ever reach
constexpr int kTerrainTop =
    static_cast<int>(kBaseHeight + kHeightAmplitude + kTerrainStrength / kTerrainSlope) + 2;

// Caves
constexpr int kCaveMinY = 4; // solid floor below this (set very negative if your world goes below y=0)
constexpr double kCaveFadeDensity = 0.4; // density at which caves reach full size (~10 blocks down)
constexpr double kTunnelFreq = 0.02;
constexpr double kTunnelWidth = 0.08; // bigger = fatter tunnels
constexpr double kCavernFreqXZ = 0.011; // low = huge caverns
constexpr double kCavernFreqY = 0.02; // higher than XZ = wide, flat-ish chambers
constexpr double kCavernThreshold = 0.5; // lower = more/bigger caverns
constexpr double kCavernSurfacePenalty = 0.25; // makes caverns rarer near the surface; they can still breach it

// Floating blobs
constexpr int kBlobMinY = 120; // must stay above kTerrainTop
constexpr int kBlobMaxY = 200;
constexpr double kBlobFreq = 0.02; // horizontal size (lower = bigger blobs)
constexpr double kBlobYSquash = 2.0; // >1 = flatter blobs
constexpr double kBlobThreshold = 0.5; // higher = fewer blobs

// Helpers
double smoothstep(double edge0, double edge1, double x) {
    const double t = std::min(1.0, std::max(0.0, (x - edge0) / (edge1 - edge0)));
    return t * t * (3.0 - 2.0 * t);
}

// [-1, 1].
double fbm3(Noise& n, double x, double y, double z, int octaves) {
    double sum = 0.0, amp = 1.0, norm = 0.0;
    for (int i = 0; i < octaves; ++i) {
        sum += amp * n.eval(x, y, z);
        norm += amp;

        x = x * 2.0 + 41.7;
        y = y * 2.0 + 17.3;
        z = z * 2.0 + 83.1;
        amp *= 0.5;
    }
    return sum / norm;
}

double groundLevelAt(Noise& n, int x, int z) {
    double sum = 0.0, amp = 1.0, norm = 0.0, freq = kHeightFreq;
    for (int i = 0; i < 3; ++i) {
        sum += amp * n.eval(x * freq, z * freq);
        norm += amp;
        freq *= 2.0;
        amp *= 0.5;
    }
    return kBaseHeight + (sum / norm) * kHeightAmplitude;
}

float terrainDensity(Noise& n, int x, int y, int z, double ground) {
    const double shape = fbm3(n, x * kTerrainFreq, y * kTerrainFreq, z * kTerrainFreq, 4);
    return static_cast<float>(shape * kTerrainStrength + (ground - y) * kTerrainSlope);
}

bool isCave(Noise& n, int x, int y, int z, float density) {
    if (y < kCaveMinY) return false;

    const double depth = std::min(1.0, std::max(0.0, density / kCaveFadeDensity));

    // Tunnels
    const double width = kTunnelWidth * (0.35 + 0.65 * depth);
    const double a = n.eval(x * kTunnelFreq + 3000.0, y * kTunnelFreq + 3000.0, z * kTunnelFreq + 3000.0);
    if (std::abs(a) < width) {
        const double b = n.eval(x * kTunnelFreq + 7000.0, y * kTunnelFreq + 7000.0, z * kTunnelFreq + 7000.0);
        if (std::abs(b) < width) return true;
    }

    // Caverns
    const double threshold = kCavernThreshold + (1.0 - depth) * kCavernSurfacePenalty;
    double c = n.eval(x * kCavernFreqXZ + 9000.0, y * kCavernFreqY + 9000.0, z * kCavernFreqXZ + 9000.0);
    if (c + 0.3 < threshold) return false; // detail below adds at most 0.3
    c += 0.3 * n.eval(x * kCavernFreqXZ * 3.0 + 9100.0, y * kCavernFreqY * 3.0 + 9100.0,
                      z * kCavernFreqXZ * 3.0 + 9100.0);
    return c > threshold;
}

bool blobAt(Noise& n, int x, int y, int z) {
    if (y < kBlobMinY || y > kBlobMaxY) return false;

    const double t = (y - kBlobMinY) / static_cast<double>(kBlobMaxY - kBlobMinY);
    const double band = 1.0 - std::abs(t * 2.0 - 1.0);
    const double threshold = kBlobThreshold + (1.0 - smoothstep(0.0, 0.4, band)) * 0.5;

    const double fy = kBlobFreq * kBlobYSquash;
    const double shape = n.eval(x * kBlobFreq + 5000.0, y * fy + 5000.0, z * kBlobFreq + 5000.0);
    if (shape + 0.15 < threshold) return false;

    const double detail = n.eval(x * kBlobFreq * 4.0 + 5100.0, y * fy * 4.0 + 5100.0, z * kBlobFreq * 4.0 + 5100.0);
    return shape + 0.15 * detail > threshold;
}

template <typename SolidAbove>
int layerBlock(SolidAbove solidAbove) {
    if (!solidAbove(1)) return kGrass;
    if (!solidAbove(2) || !solidAbove(3)) return kDirt;
    return kStone;
}

} // namespace

WorldGenerator::WorldGenerator(int seed) {
    noise = std::make_unique<OpenSimplexNoise::Noise>(seed);
}

WorldGenerator::WorldGenerator() {
    noise = std::make_unique<OpenSimplexNoise::Noise>(12345);
}

void WorldGenerator::generateChunk(Chunk& chunk, glm::ivec3 coords) const {
    chunk.dirty = true;

    constexpr int SIZE = 16;
    constexpr int PAD = 3;
    constexpr int LAYERS = SIZE + PAD;

    Noise& n = *noise;

    const int baseX = coords.x * SIZE;
    const int baseY = coords.y * SIZE;
    const int baseZ = coords.z * SIZE;

    const bool hasTerrain = baseY <= kTerrainTop;
    const bool hasBlobs = (baseY + LAYERS - 1 >= kBlobMinY) && (baseY <= kBlobMaxY);

    if (!hasTerrain && !hasBlobs) {
        for (int x = 0; x < SIZE; x++)
            for (int y = 0; y < SIZE; y++)
                for (int z = 0; z < SIZE; z++)
                    chunk.blocks[x][y][z] = kAir;
        return;
    }

    float density[SIZE][LAYERS][SIZE];
    bool blob[SIZE][LAYERS][SIZE];

    for (int x = 0; x < SIZE; x++)
        for (int z = 0; z < SIZE; z++) {
            const int wx = baseX + x;
            const int wz = baseZ + z;
            const double ground = hasTerrain ? groundLevelAt(n, wx, wz) : 0.0;

            for (int y = 0; y < LAYERS; y++) {
                const int wy = baseY + y;
                density[x][y][z] = hasTerrain ? terrainDensity(n, wx, wy, wz, ground) : -1.0f;
                blob[x][y][z] = hasBlobs && blobAt(n, wx, wy, wz);
            }
        }

    for (int x = 0; x < SIZE; x++)
        for (int y = 0; y < SIZE; y++)
            for (int z = 0; z < SIZE; z++) {
                const float d = density[x][y][z];
                int block = kAir;

                if (d > 0.0f) {
                    // Ground
                    if (!isCave(n, baseX + x, baseY + y, baseZ + z, d)) {
                        block = layerBlock([&](int k) { return density[x][y + k][z] > 0.0f; });
                    }
                } else if (blob[x][y][z]) {
                    block = layerBlock([&](int k) { return blob[x][y + k][z]; });
                }

                chunk.blocks[x][y][z] = block;
            }
}

float WorldGenerator::getDensity(int worldX, int worldY, int worldZ) {
    return terrainDensity(*noise, worldX, worldY, worldZ, groundLevelAt(*noise, worldX, worldZ));
}

bool WorldGenerator::isSolid(int worldX, int worldY, int worldZ) {
    return getBlockType(worldX, worldY, worldZ) != kAir;
}

int WorldGenerator::getBlockType(int worldX, int worldY, int worldZ) {
    Noise& n = *noise;

    const double ground = groundLevelAt(n, worldX, worldZ);
    const float d = terrainDensity(n, worldX, worldY, worldZ, ground);

    if (d > 0.0f) {
        if (isCave(n, worldX, worldY, worldZ, d)) return kAir;
        return layerBlock([&](int k) { return terrainDensity(n, worldX, worldY + k, worldZ, ground) > 0.0f; });
    }

    if (blobAt(n, worldX, worldY, worldZ)) {
        return layerBlock([&](int k) { return blobAt(n, worldX, worldY + k, worldZ); });
    }

    return kAir;
}