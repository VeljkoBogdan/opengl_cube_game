#ifndef CHUNKLOADER_H
#define CHUNKLOADER_H

#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <queue>
#include <vector>
#include <unordered_set>

#include "glm/glm.hpp"
#include "chunk.h"
#include "world.h"
#include "worldGenerator.h"
#include "../util/IVec3Hash.h"
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
    bool remesh = false;
};

constexpr int BLOCK_LAYERS[][6] {
    {0,0,0,0,0,0}, // 0 air
    {0,0,0,0,0,0}, // 1 dirt
    {1,1,2,0,1,1}, // 2 grass
    {3,3,3,3,3,3}  // 3 stone
};

class ChunkLoader {
public:
    ~ChunkLoader();

    void start(World& world, unsigned int threadCount = 0); // 0 = auto
    void stop();

    void setPlayerChunk(glm::ivec3 coord);
    void requestChunk(glm::ivec3 coord);
    void requestUnload(const std::vector<glm::ivec3>& coords);
    void queueDirty(World& world);

    void processReady(World& world, ChunkRenderer& renderer);
    void processUnload(World& world, ChunkRenderer& renderer);

private:
    void workerLoop();
    void addFace(int face, int x, int y, int z, float layer, const uint16_t (&ids)[18][18][18], std::vector<float>& verticesStorage);
    bool gatherSolid(glm::ivec3 coord, const Chunk *self, uint16_t (&solid)[18][18][18]);
    void meshFromSolid(const uint16_t(&solid)[18][18][18], std::vector<float> &vertices);
    int layerFor(int blockId, int face) const { return BLOCK_LAYERS[blockId][face]; }

    // ONE mutex guards everything below,
    // this is only touched a few times per frame, not per-vertex
    std::mutex dataMutex;

    glm::ivec3 playerChunk{0, 0, 0};

    std::priority_queue<ChunkLoadJob, std::vector<ChunkLoadJob>, ChunkLoadJobCompare> toLoad;
    std::unordered_set<glm::ivec3, IVec3Hash> pending; // queued or currently being built

    std::queue<ChunkReadyJob> ready; // built, waiting for main thread to GL-upload

    std::unordered_set<glm::ivec3, IVec3Hash> pendingUnload;
    std::queue<glm::ivec3> toUnloadQueue;

    std::vector<std::thread> workers;
    std::condition_variable wake;
    std::atomic<bool> running{false};

    WorldGenerator generator;
    World* world = nullptr;

    std::queue<glm::ivec3> toRemesh;
    std::unordered_set<glm::ivec3, IVec3Hash> remeshPending;
};

#endif