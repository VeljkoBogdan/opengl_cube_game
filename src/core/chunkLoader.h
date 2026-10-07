#ifndef CHUNKLOADER_H
#define CHUNKLOADER_H

#include <thread>
#include <mutex>
#include <atomic>
#include <queue>
#include <vector>
#include <unordered_set>

#include "glm/glm.hpp"
#include "chunk.h"
#include "world.h"
#include "worldGenerator.h"
#include "../util/IVec3Hash.h"
#include "../constants.h"
#include "../rendering/chunkRenderer.h"

struct ChunkLoadJob {
    glm::ivec3 coord;
    float priority; // squared distance to player - smaller = closer = loads first
};

struct ChunkLoadJobCompare {
    // makes the priority_queue return the SMALLEST priority first (closest chunk)
    bool operator()(const ChunkLoadJob& a, const ChunkLoadJob& b) const {
        return a.priority > b.priority;
    }
};

struct ChunkReadyJob {
    glm::ivec3 coord;
    Chunk chunk;
    std::vector<float> vertices;
};

class ChunkLoader {
public:
    ~ChunkLoader();

    void start();
    void stop();

    void setPlayerChunk(glm::ivec3 coord);
    void requestChunk(glm::ivec3 coord);
    void requestUnload(const std::vector<glm::ivec3>& coords);

    void processReady(World& world, ChunkRenderer& renderer);
    void processUnload(World& world, ChunkRenderer& renderer);

private:
    void workerLoop();
    void addFace(int face, int x, int y, int z, std::vector<float>& verticesStorage);
    void buildVertices(Chunk& chunk, std::vector<float>& vertices);

    // ONE mutex guards everything below - simpler to reason about,
    // avoids the lock-ordering bugs from before. Contention is low
    // since this is only touched a few times per frame, not per-vertex.
    std::mutex dataMutex;

    glm::ivec3 playerChunk{0, 0, 0};

    std::priority_queue<ChunkLoadJob, std::vector<ChunkLoadJob>, ChunkLoadJobCompare> toLoad;
    std::unordered_set<glm::ivec3, IVec3Hash> pending; // queued or currently being built

    std::queue<ChunkReadyJob> ready; // built, waiting for main thread to GL-upload

    std::unordered_set<glm::ivec3, IVec3Hash> pendingUnload;
    std::queue<glm::ivec3> toUnloadQueue;

    std::thread worker;
    std::atomic<bool> running{false};

    WorldGenerator generator;
};

#endif