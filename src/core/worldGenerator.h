#ifndef WORLDGENERATOR_H
#define WORLDGENERATOR_H

#include "chunk.h"
#include "glm/glm.hpp"
#include "../util/OpenSimplexNoise.h"
#include <memory>

class WorldGenerator {
public:
    WorldGenerator();
    WorldGenerator(int seed);
    void generateChunk(Chunk& chunk, glm::ivec3 coords) const;

private:
    std::unique_ptr<OpenSimplexNoise::Noise> noise;

    float getDensity(int worldX, int worldY, int worldZ);
    bool isSolid(int worldX, int worldY, int worldZ);
    int getBlockType(int worldX, int worldY, int worldZ);
};

#endif