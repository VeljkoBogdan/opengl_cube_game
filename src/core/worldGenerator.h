#ifndef WORLDGENERATOR_H
#define WORLDGENERATOR_H

#include <memory>
#include <glm/vec3.hpp>
#include <cstdint>
#include <optional>

#include "chunk.h"
#include "FeaturePass.h"
#include "FastNoise/FastNoise.h"

struct NoiseSet {
    FastNoise::SmartNode<> ground;
    FastNoise::SmartNode<> shape;
    FastNoise::SmartNode<> raw;
    int seed = 0;
};

class WorldGenerator : TerrainSampler {
public:
    WorldGenerator();
    WorldGenerator(int seed);
    void generateChunk(Chunk& chunk, glm::ivec3 coords) const;

    uint16_t blockAt(int x, int y, int z) const override;
    std::optional<int> surfaceY(int x, int z) const override;
private:
    std::unique_ptr<NoiseSet> noise;
    FeaturePass features;

    float getDensity(int worldX, int worldY, int worldZ) const;
    bool isSolid(int worldX, int worldY, int worldZ) const;
    int getBlockType(int worldX, int worldY, int worldZ) const;

    void initFeatures();
};

#endif