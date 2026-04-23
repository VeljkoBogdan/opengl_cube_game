#include "worldGenerator.h"

WorldGenerator::WorldGenerator(int seed) {
    noise = std::make_unique<OpenSimplexNoise::Noise>(seed);
}

WorldGenerator::WorldGenerator() {
    noise = std::make_unique<OpenSimplexNoise::Noise>(12345);
}

void WorldGenerator::generateChunk(Chunk& chunk, glm::ivec3 coords) {
    chunk.dirty = true;

    for(int x = 0; x < 16; x++)
    for(int z = 0; z < 16; z++) {
        int worldX = coords.x * 16 + x;
        int worldZ = coords.z * 16 + z;
        int surfaceY = getHeight(worldX, worldZ);

        for (int y = 0; y < 16; y++) {
            int worldY = coords.y * 16 + y;
            chunk.blocks[x][y][z] = getBlock(worldX, worldY, worldZ, surfaceY);
        }
    }
}

int WorldGenerator::getHeight(int worldX, int worldZ) {
    float n = noise->eval(worldX * 0.01f, worldZ * 0.01f); // returns -1 to 1
    return 64 + (int)(n * 32); // surface between y=32 and y=96
}

int WorldGenerator::getBlock(int worldX, int worldY, int worldZ, int surfaceY) {
    if (worldY > surfaceY)   return 0; // air
    if (worldY == surfaceY)  return 1; // grass
    if (worldY >= surfaceY - 3) return 2; // dirt
    return 3; // stone
}
