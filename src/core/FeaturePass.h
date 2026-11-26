#ifndef FEATUREPASS_H
#define FEATUREPASS_H

#include <climits>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <vector>

#include "glm/glm.hpp"
#include "chunk.h"
#include "Feature.h"

class TerrainSampler {
public:
    virtual ~TerrainSampler() = default;
    virtual std::optional<int> surfaceY(int x, int z) const = 0;
    virtual uint16_t blockAt(int x, int y, int z) const = 0;
};

struct PlacementRules {
    int cellSize = 6;
    float chance = 0.5f;
    int minY = INT_MIN;
    int maxY = INT_MAX;
    std::vector<uint16_t> ground;
    std::function<bool(const TerrainSampler&, glm::ivec3)> filter;

    bool accepts(const TerrainSampler& terrain, glm::ivec3 origin) const;
};

struct PlacedFeature {
    std::shared_ptr<const Feature> feature;
    PlacementRules rules;
    uint32_t salt = 0;
};

class FeaturePass {
public:
    void add(PlacedFeature placed);
    void apply(Chunk& chunk, glm::ivec3 chunkCoord, const TerrainSampler& terrain, uint64_t seed) const;

private:
    std::vector<PlacedFeature> features;
};

#endif