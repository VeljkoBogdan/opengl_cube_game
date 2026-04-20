#ifndef WORLD_H
#define WORLD_H

#include <unordered_map>

#include "../util/IVec3Hash.h"
#include "chunk.h"

class World {
public:
    Chunk& getChunk(glm::ivec3 chunkPos);
    bool hasChunk(glm::ivec3 chunkPos) const;
    void setChunk(glm::ivec3 chunkPos, Chunk chunk);

    int getBlock(glm::ivec3 worldPos) const;
    void setBlock(glm::ivec3 worldPos, int blockId);

    std::unordered_map<glm::ivec3, Chunk, IVec3Hash>& getChunks();
private:
    std::unordered_map<glm::ivec3, Chunk, IVec3Hash> chunks;

    glm::ivec3 worldToChunkPos(glm::ivec3 worldPos) const;
    glm::ivec3 worldToLocalPos(glm::ivec3 chunkPos) const;
};

#endif