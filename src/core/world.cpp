#include "world.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

Chunk &World::getChunk(glm::ivec3 chunkPos) {
    return chunks.at(chunkPos);
}

bool World::hasChunk(glm::ivec3 chunkPos) const {
    return chunks.find(chunkPos) != chunks.end();
}

void World::setChunk(glm::ivec3 chunkPos, Chunk chunk) {
    chunks[chunkPos] = chunk;
}

int World::getBlock(glm::ivec3 worldPos) const {
    glm::ivec3 chunkPos = worldToChunkPos(worldPos);
    auto it = chunks.find(chunkPos);
    if (it == chunks.end()) return 0;
    glm::ivec3 local = worldToLocalPos(worldPos);
    return it->second.blocks[local.x][local.y][local.z];
}

void World::setBlock(glm::ivec3 worldPos, int blockId) {
    glm::ivec3 chunkPos = worldToChunkPos(worldPos);
    auto it = chunks.find(chunkPos);
    if (it == chunks.end()) return;
    glm::ivec3 local = worldToLocalPos(worldPos);
    it->second.blocks[local.x][local.y][local.z] = blockId;
}

std::vector<glm::ivec3> World::getChunksToUnload(glm::ivec3 playerChunkPos, int renderDistance) {
    int half = renderDistance / 2;
    std::vector<glm::ivec3> toUnload;

    for (auto& [coord, chunk] : chunks) {
        if (abs(coord.x - playerChunkPos.x) > half ||
            abs(coord.y - playerChunkPos.y) > half ||
            abs(coord.z - playerChunkPos.z) > half) {
            toUnload.push_back(coord);
        }
    }

    return toUnload;
}

void World::unloadChunks(std::vector<glm::ivec3>& toUnload) {
    for (auto& coord : toUnload) {
        Chunk& chunk = chunks.at(coord);
        glDeleteVertexArrays(1, &chunk.VAO);
        glDeleteBuffers(1, &chunk.VBO);
        chunks.erase(coord);
    }
}

std::unordered_map<glm::ivec3, Chunk, IVec3Hash> &World::getChunks() {
    return chunks;
}

glm::ivec3 World::worldToChunkPos(glm::ivec3 worldPos) const {
    return glm::ivec3(
        glm::floor((float)worldPos.x / CHUNK_WIDTH),
        glm::floor((float)worldPos.y / CHUNK_HEIGHT),
        glm::floor((float)worldPos.z / CHUNK_LENGTH)
    );
}

glm::ivec3 World::worldToLocalPos(glm::ivec3 worldPos) const {
    return glm::ivec3(
        ((worldPos.x % CHUNK_WIDTH) + CHUNK_WIDTH) % CHUNK_WIDTH,
        ((worldPos.y % CHUNK_HEIGHT) + CHUNK_HEIGHT) % CHUNK_HEIGHT,
        ((worldPos.z % CHUNK_LENGTH) + CHUNK_LENGTH) % CHUNK_LENGTH
    );
}