#include "worldGenerator.h"

#include <algorithm>
#include <cmath>
#include <memory>

#include "FastNoise/FastNoise.h"

namespace {
using Noise = NoiseSet;

// Block ids
constexpr int kAir = 0;
constexpr int kDirt = 1;
constexpr int kGrass = 2;
constexpr int kStone = 3;

// Terrain
constexpr double kBaseHeight = 64.0; // average ground level
constexpr double kHeightAmplitude = 24.0; // how far big hills/valleys move the ground
constexpr double kHeightFreq = 0.2; // size of those hills (lower = wider)
constexpr double kTerrainFreq = 0.15; // size of 3D features (cliffs, arches, overhangs)
constexpr double kTerrainStrength = 1.0; // how strongly 3D noise fights the height bias
constexpr double kTerrainSlope = 0.04;  // density gain per block of depth
// Highest block terrain can ever reach
constexpr int kTerrainTop =
    static_cast<int>(kBaseHeight + kHeightAmplitude + kTerrainStrength / kTerrainSlope) + 2;

// Caves
constexpr int kCaveMinY = -128; // solid floor below this
constexpr double kCaveFadeRange = 24.0; // blocks over which caves taper out above kCaveMinY
constexpr double kCaveFadeDensity = 0.9; // density at which caves reach full size (~10 blocks down)
constexpr double kTunnelFreq = 0.01;
constexpr double kTunnelYScale = 1.0; // >1 = flatter, more horizontal tunnels
constexpr double kTunnelWidth = 0.03; // bigger = fatter tunnels
// Caverns
constexpr double kCavernFreqXZ = 0.6; // low = huge caverns
constexpr double kCavernFreqY = 0.9; // higher than XZ = wide, flat-ish chambers
constexpr double kCavernThreshold = 0.325; // lower = more/bigger caverns (noise is ~[-1,1], so >1 means none)
constexpr double kCavernSurfacePenalty = 0.25; // makes caverns rarer near the surface; they can still breach it

// Floating blobs
constexpr int kBlobMinY = kTerrainTop + 48; // must stay above kTerrainTop
constexpr int kBlobMaxY = kBlobMinY + 128;
constexpr double kBlobFreq = 0.9; // horizontal size (lower = bigger blobs)
constexpr double kBlobYSquash = 1.75; // >1 = flatter blobs
constexpr double kBlobThreshold = 0.8; // higher = fewer blobs

// Coordinate offsets used to decorrelate fields that share one noise source
constexpr float kOffsetTunnelA   = 3000.0f;
constexpr float kOffsetTunnelB   = 7000.0f;
constexpr float kOffsetCavern    = 9000.0f;
constexpr float kOffsetCavernDet = 9100.0f;
constexpr float kOffsetBlob      = 5000.0f;
constexpr float kOffsetBlobDet   = 5100.0f;

std::unique_ptr<NoiseSet> makeNoise(int seed) {
    auto ns = std::make_unique<NoiseSet>();
    ns->seed = seed;
    ns->raw = FastNoise::New<FastNoise::Simplex>();

    auto ground = FastNoise::New<FastNoise::FractalFBm>();
    ground->SetSource(ns->raw);
    ground->SetOctaveCount(3);
    ground->SetGain(0.5f);
    ground->SetLacunarity(2.0f);
    ns->ground = ground;

    auto shape = FastNoise::New<FastNoise::FractalFBm>();
    shape->SetSource(ns->raw);
    shape->SetOctaveCount(4);
    shape->SetGain(0.5f);
    shape->SetLacunarity(2.0f);
    ns->shape = shape;

    return ns;
}

// Helpers
double smoothstep(double edge0, double edge1, double x) {
    const double t = std::min(1.0, std::max(0.0, (x - edge0) / (edge1 - edge0)));
    return t * t * (3.0 - 2.0 * t);
}

double groundLevelAt(Noise& n, int x, int z) {
    const float v = n.ground->GenSingle2D(x * (float)kHeightFreq, z * (float)kHeightFreq, n.seed);
    return kBaseHeight + v * kHeightAmplitude;
}

float terrainDensity(Noise& n, int x, int y, int z, double ground) {
    const float shape = n.shape->GenSingle3D(x * (float)kTerrainFreq,
                                             y * (float)kTerrainFreq,
                                             z * (float)kTerrainFreq, n.seed);
    return static_cast<float>(shape * kTerrainStrength + (ground - y) * kTerrainSlope);
}

bool isCave(Noise& n, int x, int y, int z, float density) {
    // 0 at kCaveMinY, 1 at kCaveMinY + kCaveFadeRange
    const double floorFade = smoothstep(kCaveMinY, kCaveMinY + kCaveFadeRange, y);
    if (floorFade <= 0.0) return false;

    const double depth = std::min(1.0, std::max(0.0, density / kCaveFadeDensity));

    // Tunnels
    const float tf  = kTunnelFreq;
    const float tfy = tf * (float)kTunnelYScale;
    const double width = kTunnelWidth * (0.35 + 0.65 * depth) * floorFade;

    const double a = n.raw->GenSingle3D(x * tf + 3000.37f,
                                        y * tfy + 1234.91f,
                                        z * tf + 5821.53f, n.seed);
    if (std::abs(a) < width) {
        const double b = n.raw->GenSingle3D(z * tf + 7777.13f,
                                            x * tfy + 2468.29f,
                                            y * tf + 9183.71f, n.seed + 1);
        if (std::abs(b) < width) return true;
    }

    // Caverns
    const float fxz = kCavernFreqXZ, fy = kCavernFreqY;
    const double threshold = kCavernThreshold
                           + (1.0 - depth) * kCavernSurfacePenalty
                           + (1.0 - floorFade) * 0.7;
    double c = n.raw->GenSingle3D(x * fxz + kOffsetCavern,
                                  y * fy + kOffsetCavern,
                                  z * fxz + kOffsetCavern, n.seed);
    if (c + 0.3 < threshold) return false; // detail below adds at most 0.3
    c += 0.3 * n.raw->GenSingle3D(x * fxz * 3.0f + kOffsetCavernDet,
                                  y * fy * 3.0f + kOffsetCavernDet,
                                  z * fxz * 3.0f + kOffsetCavernDet, n.seed);
    return c > threshold;
}

bool blobAt(Noise& n, int x, int y, int z) {
    if (y < kBlobMinY || y > kBlobMaxY) return false;

    const double t = (y - kBlobMinY) / static_cast<double>(kBlobMaxY - kBlobMinY);
    const double band = 1.0 - std::abs(t * 2.0 - 1.0);
    const double threshold = kBlobThreshold + (1.0 - smoothstep(0.0, 0.4, band)) * 0.5;

    const float fxz = kBlobFreq, fy = kBlobFreq * kBlobYSquash;
    const double shape = n.raw->GenSingle3D(x * fxz + kOffsetBlob,
                                            y * fy + kOffsetBlob,
                                            z * fxz + kOffsetBlob, n.seed);
    if (shape + 0.15 < threshold) return false;

    const double detail = n.raw->GenSingle3D(x * fxz * 4.0f + kOffsetBlobDet,
                                             y * fy * 4.0f + kOffsetBlobDet,
                                             z * fxz * 4.0f + kOffsetBlobDet, n.seed);
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
    noise = makeNoise(seed);
}

WorldGenerator::WorldGenerator() {
    noise = makeNoise(12345);
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
    float ground2D[SIZE * SIZE];
    float shape3D[SIZE * LAYERS * SIZE];

    if (hasTerrain) {
        const float hf = kHeightFreq;
        const float tf = kTerrainFreq;

        n.ground->GenUniformGrid2D(
            ground2D,
            baseX * hf, baseZ * hf,
            SIZE, SIZE,
            hf, hf,
            n.seed);

        n.shape->GenUniformGrid3D(
            shape3D,
            baseX * tf, baseY * tf, baseZ * tf,
            SIZE, LAYERS, SIZE,
            tf, tf, tf,
            n.seed);
    }

    for (int z = 0; z < SIZE; z++)
        for (int y = 0; y < LAYERS; y++)
            for (int x = 0; x < SIZE; x++) {
                const int wy = baseY + y;

                if (hasTerrain) {
                    const double ground = kBaseHeight + ground2D[x + z * SIZE] * kHeightAmplitude;
                    const float shape = shape3D[x + y * SIZE + z * SIZE * LAYERS];
                    density[x][y][z] = (float)(shape * kTerrainStrength + (ground - wy) * kTerrainSlope);
                } else {
                    density[x][y][z] = -1.0f;
                }
                blob[x][y][z] = hasBlobs && blobAt(n, baseX + x, wy, baseZ + z);
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