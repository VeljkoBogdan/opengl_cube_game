#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "chunkLoader.h"
#include "../util/direction.h"

constexpr int chunksUploadedPerFrame = 8;
constexpr int chunksUnloadedPerFrame = 8;

void ChunkLoader::addFace(int face, int x, int y, int z, std::vector<float>& verticesStorage) {
    switch (face) {
        case DOWN:
            verticesStorage.insert(verticesStorage.end(), {
                x-0.5f, y-0.5f, z-0.5f,  0.0f, -1.0f, 0.0f,  0.0f, 0.0f,
                x+0.5f, y-0.5f, z-0.5f,  0.0f, -1.0f, 0.0f,  1.0f, 0.0f,
                x+0.5f, y-0.5f, z+0.5f,  0.0f, -1.0f, 0.0f,  1.0f, 1.0f,
                x+0.5f, y-0.5f, z+0.5f,  0.0f, -1.0f, 0.0f,  1.0f, 1.0f,
                x-0.5f, y-0.5f, z+0.5f,  0.0f, -1.0f, 0.0f,  0.0f, 1.0f,
                x-0.5f, y-0.5f, z-0.5f,  0.0f, -1.0f, 0.0f,  0.0f, 0.0f
            });
            break;
        case UP:
            verticesStorage.insert(verticesStorage.end(), {
                x-0.5f, y+0.5f, z-0.5f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f,
                x-0.5f, y+0.5f, z+0.5f,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f,
                x+0.5f, y+0.5f, z+0.5f,  0.0f, 1.0f, 0.0f,  1.0f, 1.0f,
                x+0.5f, y+0.5f, z+0.5f,  0.0f, 1.0f, 0.0f,  1.0f, 1.0f,
                x+0.5f, y+0.5f, z-0.5f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f,
                x-0.5f, y+0.5f, z-0.5f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f
            });
            break;
        case LEFT:
            verticesStorage.insert(verticesStorage.end(), {
                x-0.5f, y-0.5f, z-0.5f,  -1.0f, 0.0f, 0.0f,  1.0f, 0.0f,
                x-0.5f, y-0.5f, z+0.5f,  -1.0f, 0.0f, 0.0f,  0.0f, 0.0f,
                x-0.5f, y+0.5f, z-0.5f,  -1.0f, 0.0f, 0.0f,  1.0f, 1.0f,
                x-0.5f, y+0.5f, z-0.5f,  -1.0f, 0.0f, 0.0f,  1.0f, 1.0f,
                x-0.5f, y-0.5f, z+0.5f,  -1.0f, 0.0f, 0.0f,  0.0f, 0.0f,
                x-0.5f, y+0.5f, z+0.5f,  -1.0f, 0.0f, 0.0f,  0.0f, 1.0f
            });
            break;
        case RIGHT:
            verticesStorage.insert(verticesStorage.end(), {
                x+0.5f, y+0.5f, z-0.5f,  1.0f, 0.0f, 0.0f,  0.0f, 1.0f,
                x+0.5f, y-0.5f, z+0.5f,  1.0f, 0.0f, 0.0f,  1.0f, 0.0f,
                x+0.5f, y-0.5f, z-0.5f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f,
                x+0.5f, y-0.5f, z+0.5f,  1.0f, 0.0f, 0.0f,  1.0f, 0.0f,
                x+0.5f, y+0.5f, z-0.5f,  1.0f, 0.0f, 0.0f,  0.0f, 1.0f,
                x+0.5f, y+0.5f, z+0.5f,  1.0f, 0.0f, 0.0f,  1.0f, 1.0f
            });
            break;
        case BACK:
            verticesStorage.insert(verticesStorage.end(), {
                x-0.5f, y-0.5f, z+0.5f,  0.0f, 0.0f, 1.0f,  1.0f, 0.0f,
                x+0.5f, y-0.5f, z+0.5f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f,
                x-0.5f, y+0.5f, z+0.5f,  0.0f, 0.0f, 1.0f,  1.0f, 1.0f,
                x-0.5f, y+0.5f, z+0.5f,  0.0f, 0.0f, 1.0f,  1.0f, 1.0f,
                x+0.5f, y-0.5f, z+0.5f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f,
                x+0.5f, y+0.5f, z+0.5f,  0.0f, 0.0f, 1.0f,  0.0f, 1.0f
            });
            break;
        case FRONT:
            verticesStorage.insert(verticesStorage.end(), {
                x-0.5f, y-0.5f, z-0.5f,  0.0f, 0.0f, -1.0f,  0.0f, 0.0f,
                x+0.5f, y+0.5f, z-0.5f,  0.0f, 0.0f, -1.0f,  1.0f, 1.0f,
                x+0.5f, y-0.5f, z-0.5f,  0.0f, 0.0f, -1.0f,  1.0f, 0.0f,
                x-0.5f, y-0.5f, z-0.5f,  0.0f, 0.0f, -1.0f,  0.0f, 0.0f,
                x-0.5f, y+0.5f, z-0.5f,  0.0f, 0.0f, -1.0f,  0.0f, 1.0f,
                x+0.5f, y+0.5f, z-0.5f,  0.0f, 0.0f, -1.0f,  1.0f, 1.0f
            });
            break;
        default:
            break;
    }
}

// vertices are chunk local, the world offset is applied on the gpu
void ChunkLoader::buildVertices(Chunk& chunk, std::vector<float>& vertices) {
    auto getBlock = [&](int x, int y, int z) -> int {
        if (x >= 0 && x < 16 && y >= 0 && y < 16 && z >= 0 && z < 16)
            return chunk.blocks[x][y][z];
        return 0;
    };

    for (int x = 0; x < CHUNK_WIDTH; x++)
    for (int y = 0; y < CHUNK_HEIGHT; y++)
    for (int z = 0; z < CHUNK_LENGTH; z++) {
        if (chunk.blocks[x][y][z] == 0) continue;

        if (getBlock(x, y-1, z) == 0) addFace(DOWN,  x, y, z, vertices);
        if (getBlock(x, y+1, z) == 0) addFace(UP,    x, y, z, vertices);
        if (getBlock(x-1, y, z) == 0) addFace(LEFT,  x, y, z, vertices);
        if (getBlock(x+1, y, z) == 0) addFace(RIGHT, x, y, z, vertices);
        if (getBlock(x, y, z-1) == 0) addFace(FRONT, x, y, z, vertices);
        if (getBlock(x, y, z+1) == 0) addFace(BACK,  x, y, z, vertices);
    }
}

ChunkLoader::~ChunkLoader() {
    stop();
}

void ChunkLoader::start() {
    running = true;
    worker = std::thread(&ChunkLoader::workerLoop, this);
}

void ChunkLoader::stop() {
    running = false;
    if (worker.joinable()) worker.join();
}

void ChunkLoader::setPlayerChunk(glm::ivec3 coord) {
    std::lock_guard lock(dataMutex);
    playerChunk = coord;
}

void ChunkLoader::requestChunk(glm::ivec3 coord) {
    std::lock_guard lock(dataMutex);

    pendingUnload.erase(coord);

    if (pending.count(coord)) return;
    pending.insert(coord);

    glm::ivec3 diff = coord - playerChunk;
    float priority = (float)(diff.x*diff.x + diff.y*diff.y + diff.z*diff.z);
    toLoad.push({coord, priority});
}

void ChunkLoader::requestUnload(const std::vector<glm::ivec3>& coords) {
    std::lock_guard lock(dataMutex);

    for (const auto& coord : coords) {
        pending.erase(coord);

        pendingUnload.insert(coord);
        toUnloadQueue.push(coord);
    }
}

void ChunkLoader::processReady(World& world, ChunkRenderer& chunkRenderer) {
    std::vector<ChunkReadyJob> batch;
    {
        std::lock_guard lock(dataMutex);
        int taken = 0;
        while (!ready.empty() && taken < chunksUploadedPerFrame) {
            batch.push_back(std::move(ready.front()));
            ready.pop();
            taken++;
        }
    }

    for (auto& job : batch) {
        bool cancelled;
        {
            std::lock_guard lock(dataMutex);
            cancelled = pendingUnload.count(job.coord) > 0;
        }
        if (cancelled) continue;

        // copy the mesh into the shared buffer
        chunkRenderer.upload(job.chunk, job.vertices);

        job.chunk.dirty = false;
        world.setChunk(job.coord, std::move(job.chunk));

        std::lock_guard lock(dataMutex);
        pending.erase(job.coord);
    }
}

void ChunkLoader::processUnload(World& world, ChunkRenderer& chunkRenderer) {
    std::vector<glm::ivec3> batch;
    {
        std::lock_guard lock(dataMutex);
        int taken = 0;
        while (!toUnloadQueue.empty() && taken < chunksUnloadedPerFrame) {
            glm::ivec3 coord = toUnloadQueue.front();
            toUnloadQueue.pop();
            if (pendingUnload.count(coord)) {
                pendingUnload.erase(coord);
                batch.push_back(coord);
            }
            taken++;
        }
    }

    auto& chunks = world.getChunks();
    for (auto& coord : batch) {
        auto it = chunks.find(coord);
        if (it == chunks.end()) continue;
        chunkRenderer.release(it->second); // give the chunk vertex back to the arena
        world.unloadChunk(coord);
    }
}

void ChunkLoader::workerLoop() {
    while (running) {
        ChunkLoadJob job;
        bool hasJob = false;
        bool cancelled = false;

        {
            std::lock_guard lock(dataMutex);
            if (!toLoad.empty()) {
                job = toLoad.top();
                toLoad.pop();
                hasJob = true;
                cancelled = pendingUnload.count(job.coord) > 0;
            }
        }

        if (!hasJob) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            continue;
        }

        if (cancelled) {
            std::lock_guard lock(dataMutex);
            pending.erase(job.coord);
            continue;
        }

        Chunk chunk;
        generator.generateChunk(chunk, job.coord);

        std::vector<float> vertices;
        buildVertices(chunk, vertices);

        std::lock_guard lock(dataMutex);
        ready.push({job.coord, std::move(chunk), std::move(vertices)});
    }
}