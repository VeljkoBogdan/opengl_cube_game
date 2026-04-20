#ifndef WORLDGENERATOR_H
#define WORLDGENERATOR_H

#include "chunk.h"
#include "glm/glm.hpp"
#include "../util/OpenSimplexNoise.h"
#include <memory>

class WorldGenerator {
public:
    WorldGenerator(int seed);
    void generateChunk(Chunk& chunk, glm::ivec3 coords);

private:
    int seed;
    std::unique_ptr<OpenSimplexNoise::Noise> noise;

    int getHeight(int worldX, int worldZ);
    int getBlock(int worldX, int worldY, int worldZ, int surfaceY);
};

#endif