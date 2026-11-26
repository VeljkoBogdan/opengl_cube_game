#include "FeaturePass.h"

#include <algorithm>

static int floorDiv(int a, int b) {
    int q = a / b;
    return (a % b != 0 && ((a < 0) != (b < 0))) ? q - 1 : q;
}

bool PlacementRules::accepts(const TerrainSampler& terrain, glm::ivec3 origin) const {
    if (!ground.empty()) {
        uint16_t below = terrain.blockAt(origin.x, origin.y - 1, origin.z);
        if (std::find(ground.begin(), ground.end(), below) == ground.end()) return false;
    }
    return !filter || filter(terrain, origin);
}

void FeaturePass::add(PlacedFeature placed) {
    features.push_back(std::move(placed));
}

void FeaturePass::apply(Chunk& chunk, glm::ivec3 chunkCoord, const TerrainSampler& terrain, uint64_t seed) const {
    const glm::ivec3 chunkMin = chunkCoord * glm::ivec3(CHUNK_WIDTH, CHUNK_HEIGHT, CHUNK_LENGTH);
    const glm::ivec3 chunkMax = chunkMin + glm::ivec3(CHUNK_WIDTH - 1, CHUNK_HEIGHT - 1, CHUNK_LENGTH - 1);

    for (const PlacedFeature& placed : features) {
        const FeatureExtent extent = placed.feature->extent();
        const PlacementRules& rules = placed.rules;
        const int cell = rules.cellSize;

        const int minX = chunkMin.x - extent.radius;
        const int maxX = chunkMax.x + extent.radius;
        const int minZ = chunkMin.z - extent.radius;
        const int maxZ = chunkMax.z + extent.radius;

        for (int cz = floorDiv(minZ, cell); cz <= floorDiv(maxZ, cell); cz++)
        for (int cx = floorDiv(minX, cell); cx <= floorDiv(maxX, cell); cx++) {
            Rng rng(hashCoords(seed, cx, cz, placed.salt));
            if (!rng.chance(rules.chance)) continue;

            const int x = cx * cell + rng.nextInt(0, cell - 1);
            const int z = cz * cell + rng.nextInt(0, cell - 1);
            if (x < minX || x > maxX || z < minZ || z > maxZ) continue;

            std::optional<int> surface = terrain.surfaceY(x, z);
            if (!surface) continue;

            const glm::ivec3 origin(x, *surface + 1, z);
            if (origin.y < rules.minY || origin.y > rules.maxY) continue;
            if (origin.y - extent.below > chunkMax.y || origin.y + extent.above < chunkMin.y) continue;
            if (!rules.accepts(terrain, origin)) continue;

            FeatureWriter writer{chunk, chunkMin, origin};
            placed.feature->generate(writer, rng);
        }
    }
}