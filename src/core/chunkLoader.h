#ifndef CHUNKLOADER_H
#define CHUNKLOADER_H

#include <thread>
#include <mutex>
#include <atomic>
#include <queue>
#include <unordered_set>

#include "glm/glm.hpp"
#include "chunk.h"
#include "world.h"
#include "worldGenerator.h"
#include "../util/IVec3Hash.h"

struct ChunkLoadJob {
    glm::ivec3 coord;
};

struct ChunkReadyJob {
    glm::ivec3 coord;
    Chunk chunk;
    std::vector<float> vertices;
};

class ChunkLoader {
public:
    ~ChunkLoader();

    void requestChunk(glm::ivec3 coord);
    void processReady(World& world);
    void unloadChunks(std::vector<glm::ivec3>& toUnload);

    void start();
    void stop();
private:
    void workerLoop();
    void addFace(int face, int x, int y, int z, std::vector<float>& verticesStorage);
    void buildVertices(Chunk& chunk, std::vector<float>& vertices);

    std::unordered_set<glm::ivec3, IVec3Hash> pending;

    std::thread worker;
    std::atomic<bool> running;

    std::queue<ChunkLoadJob> toLoad;
    std::mutex toLoadMutex;

    std::queue<ChunkReadyJob> ready;
    std::mutex readyMutex;

    WorldGenerator generator;
};

#endif