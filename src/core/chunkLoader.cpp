#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "chunkLoader.h"
#include "../util/direction.h"

void ChunkLoader::addFace(int face, int x, int y, int z, std::vector<float>& verticesStorage) {
    switch (face) {
        case Direction::DOWN:
            verticesStorage.insert(verticesStorage.end(), {
                x-0.5f, y-0.5f, z-0.5f,  0.0f, -1.0f, 0.0f,
                x+0.5f, y-0.5f, z-0.5f,  0.0f, -1.0f, 0.0f,
                x+0.5f, y-0.5f, z+0.5f,  0.0f, -1.0f, 0.0f,
                x+0.5f, y-0.5f, z+0.5f,  0.0f, -1.0f, 0.0f,
                x-0.5f, y-0.5f, z+0.5f,  0.0f, -1.0f, 0.0f,
                x-0.5f, y-0.5f, z-0.5f,  0.0f, -1.0f, 0.0f
            });
            break;
        case Direction::UP:
            verticesStorage.insert(verticesStorage.end(), {
                x-0.5f, y+0.5f, z-0.5f,  0.0f, 1.0f, 0.0f,
                x-0.5f, y+0.5f, z+0.5f,  0.0f, 1.0f, 0.0f,
                x+0.5f, y+0.5f, z+0.5f,  0.0f, 1.0f, 0.0f,
                x+0.5f, y+0.5f, z+0.5f,  0.0f, 1.0f, 0.0f,
                x+0.5f, y+0.5f, z-0.5f,  0.0f, 1.0f, 0.0f,
                x-0.5f, y+0.5f, z-0.5f,  0.0f, 1.0f, 0.0f
            });
            break;
        case Direction::LEFT:
            verticesStorage.insert(verticesStorage.end(), {
                x-0.5f, y-0.5f, z-0.5f,  -1.0f, 0.0f, 0.0f,
                x-0.5f, y-0.5f, z+0.5f,  -1.0f, 0.0f, 0.0f,
                x-0.5f, y+0.5f, z-0.5f,  -1.0f, 0.0f, 0.0f,
                x-0.5f, y+0.5f, z-0.5f,  -1.0f, 0.0f, 0.0f,
                x-0.5f, y-0.5f, z+0.5f,  -1.0f, 0.0f, 0.0f,
                x-0.5f, y+0.5f, z+0.5f,  -1.0f, 0.0f, 0.0f
            });
            break;
        case Direction::RIGHT:
            verticesStorage.insert(verticesStorage.end(), {
                x+0.5f, y+0.5f, z-0.5f,  1.0f, 0.0f, 0.0f,
                x+0.5f, y-0.5f, z+0.5f,  1.0f, 0.0f, 0.0f,
                x+0.5f, y-0.5f, z-0.5f,  1.0f, 0.0f, 0.0f,
                x+0.5f, y-0.5f, z+0.5f,  1.0f, 0.0f, 0.0f,
                x+0.5f, y+0.5f, z-0.5f,  1.0f, 0.0f, 0.0f,
                x+0.5f, y+0.5f, z+0.5f,  1.0f, 0.0f, 0.0f
            });
            break;
        case Direction::BACK:
            verticesStorage.insert(verticesStorage.end(), {
                x-0.5f, y-0.5f, z+0.5f,  0.0f, 0.0f, 1.0f,
                x+0.5f, y-0.5f, z+0.5f,  0.0f, 0.0f, 1.0f,
                x-0.5f, y+0.5f, z+0.5f,  0.0f, 0.0f, 1.0f,
                x-0.5f, y+0.5f, z+0.5f,  0.0f, 0.0f, 1.0f,
                x+0.5f, y-0.5f, z+0.5f,  0.0f, 0.0f, 1.0f,
                x+0.5f, y+0.5f, z+0.5f,  0.0f, 0.0f, 1.0f
            });
            break;
        case Direction::FRONT:
            verticesStorage.insert(verticesStorage.end(), {
                x-0.5f, y-0.5f, z-0.5f,  0.0f, 0.0f, -1.0f,
                x+0.5f, y+0.5f, z-0.5f,  0.0f, 0.0f, -1.0f,
                x+0.5f, y-0.5f, z-0.5f,  0.0f, 0.0f, -1.0f,
                x-0.5f, y-0.5f, z-0.5f,  0.0f, 0.0f, -1.0f,
                x-0.5f, y+0.5f, z-0.5f,  0.0f, 0.0f, -1.0f,
                x+0.5f, y+0.5f, z-0.5f,  0.0f, 0.0f, -1.0f
            });
            break;
        default:
            break;
    }
}

void ChunkLoader::buildVertices(Chunk& chunk, std::vector<float>& vertices) {
    auto getBlock = [&](int x, int y, int z) -> int {
        if (x >= 0 && x < 16 &&
            y >= 0 && y < 16 &&
            z >= 0 && z < 16)
            return chunk.blocks[x][y][z];

        // glm::ivec3 neighborPos = ;
        // int nx = x, ny = y, nz = z;

        // if (x < 0)  { neighborPos.x--; nx = 15; }
        // if (x >= 16){ neighborPos.x++; nx = 0;  }
        // if (y < 0)  { neighborPos.y--; ny = 15; }
        // if (y >= 16){ neighborPos.y++; ny = 0;  }
        // if (z < 0)  { neighborPos.z--; nz = 15; }
        // if (z >= 16){ neighborPos.z++; nz = 0;  }

        // auto it = world.getChunks().find(neighborPos);
        // if (it == world.getChunks().end()) return 0; // no neighbor, treat as air
        // return it->second.blocks[nx][ny][nz];

        return 0;
    };
    
    for (int x = 0; x < CHUNK_WIDTH; x++)
    for (int y = 0; y < CHUNK_HEIGHT; y++)
    for (int z = 0; z < CHUNK_LENGTH; z++) {
        if (chunk.blocks[x][y][z] == 0) continue;

        if (getBlock(x, y-1, z) == 0) addFace(Direction::DOWN,  x, y, z, vertices);
        if (getBlock(x, y+1, z) == 0) addFace(Direction::UP,    x, y, z, vertices);
        if (getBlock(x-1, y, z) == 0) addFace(Direction::LEFT,  x, y, z, vertices);
        if (getBlock(x+1, y, z) == 0) addFace(Direction::RIGHT, x, y, z, vertices);
        if (getBlock(x, y, z-1) == 0) addFace(Direction::FRONT, x, y, z, vertices);
        if (getBlock(x, y, z+1) == 0) addFace(Direction::BACK,  x, y, z, vertices);
    }
}

ChunkLoader::~ChunkLoader() {
    stop();
}

void ChunkLoader::requestChunk(glm::ivec3 coord)
{
    std::lock_guard<std::mutex> lock(toLoadMutex);
    if (pending.count(coord)) return; // return if already queued
    pending.insert(coord);
    toLoad.push({coord});
}

void ChunkLoader::processReady(World &world) {
    std::lock_guard<std::mutex> lock(readyMutex);

    int processed = 0;
    while (!ready.empty() && processed < 8) {
        ChunkReadyJob job = std::move(ready.front());
        ready.pop();

        glGenVertexArrays(1, &job.chunk.VAO);
        glGenBuffers(1, &job.chunk.VBO);

        glBindVertexArray(job.chunk.VAO);
        glBindBuffer(GL_ARRAY_BUFFER, job.chunk.VBO);
        glBufferData(GL_ARRAY_BUFFER, job.vertices.size() * sizeof(float), job.vertices.data(), GL_DYNAMIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        job.chunk.dirty = false;
        job.chunk.vertexCount = job.vertices.size() / 6;
        world.setChunk(job.coord, std::move(job.chunk));

        processed++;
    }
}

void ChunkLoader::unloadChunks(std::vector<glm::ivec3> &toUnload) {
    // clear the pending queue
    std::lock_guard<std::mutex> lock(toLoadMutex);

    for (auto& coord : toUnload) pending.erase(coord);

    std::queue<ChunkLoadJob> filtered;
    while (!toLoad.empty()) {
        ChunkLoadJob job = toLoad.front();
        toLoad.pop();
        if (pending.count(job.coord)) filtered.push(job);
    }
    toLoad = std::move(filtered);

    // clear the ready queue
    std::lock_guard<std::mutex> readyLock(readyMutex);
    std::unordered_set<glm::ivec3, IVec3Hash> unloadSet(toUnload.begin(), toUnload.end());
    std::queue<ChunkReadyJob> filteredReady;
    while (!ready.empty()) {
        ChunkReadyJob job = std::move(ready.front());
        ready.pop();
        if (!unloadSet.count(job.coord)) filteredReady.push(job);
    }
    ready = std::move(filteredReady);
}

void ChunkLoader::start() {
    running = true;
    worker = std::thread(&ChunkLoader::workerLoop, this);
}

void ChunkLoader::stop() {
    running = false;
    if (worker.joinable()) worker.join();
}

void ChunkLoader::workerLoop() {
    while (running) {
        ChunkLoadJob job;
        bool hasJob = false;

        {
            std::lock_guard<std::mutex> lock(toLoadMutex);
            if (!toLoad.empty()) {
                job = toLoad.front();
                toLoad.pop();
                hasJob = true;
            }
        }

        if (!hasJob) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            continue;
        }

        Chunk chunk;
        generator.generateChunk(chunk, job.coord);

        std::vector<float> vertices;
        buildVertices(chunk, vertices);

        {
            std::lock_guard<std::mutex> lock(readyMutex);
            ready.push({
                job.coord,
                std::move(chunk),
                std::move(vertices)
            });
        }
    }
}