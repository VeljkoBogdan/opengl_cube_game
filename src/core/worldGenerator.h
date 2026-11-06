#ifndef WORLDGENERATOR_H
#define WORLDGENERATOR_H

#include <memory>
#include <glm/vec3.hpp>

#include "chunk.h"
#include "FastNoise/FastNoise.h"

struct NoiseSet {
    FastNoise::SmartNode<> ground;
    FastNoise::SmartNode<> shape;
    FastNoise::SmartNode<> raw;
    int seed = 0;
};

class WorldGenerator {
public:
    WorldGenerator();
    WorldGenerator(int seed);
    void generateChunk(Chunk& chunk, glm::ivec3 coords) const;

private:
    std::unique_ptr<NoiseSet> noise;

    float getDensity(int worldX, int worldY, int worldZ);
    bool isSolid(int worldX, int worldY, int worldZ);
    int getBlockType(int worldX, int worldY, int worldZ);
};

#endif