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

bool ChunkLoader::gatherSolid(glm::ivec3 coord, const Chunk *self, uint8_t(&solid)[18][18][18]) {
    std::memset(solid, 0, sizeof(solid));

    std::lock_guard lock(dataMutex);
    auto& chunks = world->getChunks();

    if (!self) {
        auto it = chunks.find(coord);
        if (it == chunks.end()) return false;
        self = &it->second;
    }

    for (int x = 0; x < CHUNK_WIDTH; x++)
        for (int y = 0; y < CHUNK_HEIGHT; y++)
            for (int z = 0; z < CHUNK_LENGTH; z++)
                solid[x+1][y+1][z+1] = self->blocks[x][y][z] != 0;

    auto getChunk = [&](int dx, int dy, int dz) -> Chunk* {
        auto it = chunks.find(coord + glm::ivec3(dx, dy, dz));
        return it == chunks.end() ? nullptr : &it->second;
    };
    Chunk* nx = getChunk(-1, 0, 0);
    Chunk* px = getChunk(1, 0, 0);
    Chunk* ny = getChunk(0, -1, 0);
    Chunk* py = getChunk(0, 1, 0);
    Chunk* nz = getChunk(0, 0, -1);
    Chunk* pz = getChunk(0, 0, 1);

    for (int a = 0; a < 16; a++)
        for (int b = 0; b < 16; b++) {
            if (nx) solid[0][a+1][b+1]  = nx->blocks[15][a][b] != 0;
            if (px) solid[17][a+1][b+1] = px->blocks[0][a][b]  != 0;
            if (ny) solid[a+1][0][b+1]  = ny->blocks[a][15][b] != 0;
            if (py) solid[a+1][17][b+1] = py->blocks[a][0][b]  != 0;
            if (nz) solid[a+1][b+1][0]  = nz->blocks[a][b][15] != 0;
            if (pz) solid[a+1][b+1][17] = pz->blocks[a][b][0]  != 0;
        }
    return true;
}

void ChunkLoader::meshFromSolid(const uint8_t(&solid)[18][18][18], std::vector<float> &vertices) {
    for (int x = 0; x < CHUNK_WIDTH; x++)
    for (int y = 0; y < CHUNK_HEIGHT; y++)
    for (int z = 0; z < CHUNK_LENGTH; z++) {
        if (!solid[x+1][y+1][z+1]) continue;

        if (!solid[x+1][y][z+1])   addFace(DOWN,  x, y, z, vertices);
        if (!solid[x+1][y+2][z+1]) addFace(UP,    x, y, z, vertices);
        if (!solid[x][y+1][z+1])   addFace(LEFT,  x, y, z, vertices);
        if (!solid[x+2][y+1][z+1]) addFace(RIGHT, x, y, z, vertices);
        if (!solid[x+1][y+1][z])   addFace(FRONT, x, y, z, vertices);
        if (!solid[x+1][y+1][z+2]) addFace(BACK,  x, y, z, vertices);
    }
}


ChunkLoader::~ChunkLoader() {
    stop();
}

void ChunkLoader::start(World& world) {
    running = true;
    worker = std::thread(&ChunkLoader::workerLoop, this);
    this->world = &world;
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

void ChunkLoader::queueDirty(World &world) {
    std::lock_guard lock(dataMutex);
    for (auto& [coord, chunk] : world.getChunks()) {
        if (!chunk.dirty || remeshPending.count(coord)) continue;
        chunk.dirty = false;
        remeshPending.insert(coord);
        toRemesh.push(coord);
    }
}

static const glm::ivec3 neighborDirs[6] = {
    {-1, 0, 0}, {1, 0, 0}, {0, -1, 0}, {0, 1, 0}, {0, 0, -1}, {0, 0, 1}
};

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

    auto& chunks = world.getChunks();
    for (auto& job : batch) {
        if (job.remesh) {
            {
                std::lock_guard lock(dataMutex);
                remeshPending.erase(job.coord);
            }
            auto it = chunks.find(job.coord);
            if (it == chunks.end()) continue;
            chunkRenderer.release(it->second);
            chunkRenderer.upload(it->second, job.vertices);
            continue;
        }

        bool cancelled;
        {
            std::lock_guard lock(dataMutex);
            cancelled = pendingUnload.count(job.coord) > 0;
        }
        if (cancelled) continue;

        // copy the mesh into the shared buffer
        chunkRenderer.upload(job.chunk, job.vertices);
        job.chunk.dirty = false;
        {
            std::lock_guard lock(dataMutex);
            world.setChunk(job.coord, std::move(job.chunk));
            pending.erase(job.coord);
        }

        for (const glm::ivec3& d : neighborDirs) {
            auto it = chunks.find(job.coord + d);
            if (it != chunks.end() && it->second.vertexCount > 0) it->second.dirty = true;
        }
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
        {
            std::lock_guard lock(dataMutex);
            world.unloadChunk(coord);
        }
    }
}

void ChunkLoader::workerLoop() {
    while (running) {
        ChunkLoadJob job;
        glm::ivec3 remeshCoord{0};
        bool hasJob = false;
        bool isRemesh = false;
        bool cancelled = false;

        {
            std::lock_guard lock(dataMutex);
            if (!toRemesh.empty()) {
                remeshCoord = toRemesh.front();
                toRemesh.pop();
                hasJob = isRemesh = true;
            } else if (!toLoad.empty()) {
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

        if (isRemesh) {
            uint8_t solid[18][18][18];
            std::vector<float> vertices;
            bool ok = gatherSolid(remeshCoord, nullptr, solid);
            if (ok) meshFromSolid(solid, vertices);

            std::lock_guard lock(dataMutex);
            if (ok) ready.push({remeshCoord, Chunk(), std::move(vertices), true});
            else remeshPending.erase(remeshCoord);
            continue;
        }

        if (cancelled) {
            std::lock_guard lock(dataMutex);
            pending.erase(job.coord);
            continue;
        }

        Chunk chunk;
        generator.generateChunk(chunk, job.coord);

        uint8_t solid[18][18][18];
        gatherSolid(job.coord, &chunk, solid);
        std::vector<float> vertices;
        meshFromSolid(solid, vertices);

        std::lock_guard lock(dataMutex);
        ready.push({job.coord, std::move(chunk), std::move(vertices)});
    }
}