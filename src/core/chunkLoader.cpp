#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "chunkLoader.h"
#include "../util/direction.h"

constexpr int chunksUploadedPerFrame = 64;
constexpr int chunksUnloadedPerFrame = 64;

struct FaceDef { glm::ivec3 n, t, b; };

static FaceDef faceDef(int face) {
    switch (face) {
        case DOWN:  return {{ 0,-1, 0}, { 1, 0, 0}, { 0, 0, 1}};
        case UP:    return {{ 0, 1, 0}, { 1, 0, 0}, { 0, 0,-1}};
        case LEFT:  return {{-1, 0, 0}, { 0, 0, 1}, { 0, 1, 0}};
        case RIGHT: return {{ 1, 0, 0}, { 0, 0,-1}, { 0, 1, 0}};
        case BACK:  return {{ 0, 0, 1}, { 1, 0, 0}, { 0, 1, 0}};
        case FRONT: return {{ 0, 0,-1}, {-1, 0, 0}, { 0, 1, 0}};
    }
    return {};
}

void ChunkLoader::addFace(int face, int x, int y, int z, uint32_t layer,
                          const uint16_t (&ids)[18][18][18], std::vector<uint32_t>& verticesStorage) {
    const FaceDef f = faceDef(face);
    const glm::ivec3 front = glm::ivec3(x + 1, y + 1, z + 1) + f.n;
    auto occ = [&](glm::ivec3 p) { return ids[p.x][p.y][p.z] != 0 ? 1 : 0; };

    static const int su[4] = {-1,  1, 1, -1};
    static const int sv[4] = {-1, -1, 1,  1};

    uint32_t packed[4];
    float ao[4];
    for (int i = 0; i < 4; i++) {
        glm::ivec3 ts = f.t * su[i], bs = f.b * sv[i];
        int s1 = occ(front + ts);
        int s2 = occ(front + bs);
        int c  = occ(front + ts + bs);
        ao[i] = (s1 && s2) ? 0 : 3 - (s1 + s2 + c);

        // packing
        // x(5) y(5) z(5) | face(3) | corner(2) | layer(8) | ao(2)
        const glm::ivec3 p = glm::ivec3(x, y, z) + (f.n + ts + bs + glm::ivec3(1)) / 2;
        packed[i] = (uint32_t)p.x |
            ((uint32_t)p.y << 5) |
            ((uint32_t)p.z << 10) |
            ((uint32_t)face << 15) |
            ((uint32_t)i << 18) |
            (layer << 20) |
            ((uint32_t)ao[i] << 28);
    }

    static const int tris[2][6] = {{0, 1, 2, 0, 2, 3}, {1, 2, 3, 1, 3, 0}};
    const int* order = (ao[0] + ao[2] > ao[1] + ao[3]) ? tris[0] : tris[1];

    for (int k = 0; k < 6; k++) verticesStorage.push_back(packed[order[k]]);
}

bool ChunkLoader::gatherSolid(glm::ivec3 coord, const Chunk *self, uint16_t (&solid)[18][18][18]) {
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
            std::copy_n(self->blocks[x][y], CHUNK_LENGTH, &solid[x + 1][y + 1][1]);

    auto span = [](int d, int& dst, int& len, int& src) {
        if (d < 0)      { dst = 0;  len = 1;  src = 15; }
        else if (d > 0) { dst = 17; len = 1;  src = 0;  }
        else            { dst = 1;  len = 16; src = 0;  }
    };
    for (int dx = -1; dx <= 1; dx++)
        for (int dy = -1; dy <= 1; dy++)
            for (int dz = -1; dz <= 1; dz++) {
                if (!dx && !dy && !dz) continue;
                auto it = chunks.find(coord + glm::ivec3(dx, dy, dz));
                if (it == chunks.end()) continue;
                int px, nx, sx, py, ny, sy, pz, nz, sz;
                span(dx, px, nx, sx); span(dy, py, ny, sy); span(dz, pz, nz, sz);
                for (int i = 0; i < nx; i++)
                    for (int j = 0; j < ny; j++)
                        std::copy_n(&it->second.blocks[sx + i][sy + j][sz], nz, &solid[px + i][py + j][pz]);
            }

    return true;
}

void ChunkLoader::meshFromSolid(const uint16_t(&solid)[18][18][18], std::vector<uint32_t> &vertices) {
    for (int x = 0; x < CHUNK_WIDTH; x++)
    for (int y = 0; y < CHUNK_HEIGHT; y++)
    for (int z = 0; z < CHUNK_LENGTH; z++) {
        if (!solid[x+1][y+1][z+1]) continue;

        if (!solid[x+1][y][z+1])   addFace(DOWN,  x, y, z, (uint32_t)layerFor(solid[x+1][y+1][z+1], DOWN), solid, vertices);
        if (!solid[x+1][y+2][z+1]) addFace(UP,    x, y, z, (uint32_t)layerFor(solid[x+1][y+1][z+1], UP), solid, vertices);
        if (!solid[x][y+1][z+1])   addFace(LEFT,  x, y, z, (uint32_t)layerFor(solid[x+1][y+1][z+1], LEFT), solid, vertices);
        if (!solid[x+2][y+1][z+1]) addFace(RIGHT, x, y, z, (uint32_t)layerFor(solid[x+1][y+1][z+1], RIGHT), solid, vertices);
        if (!solid[x+1][y+1][z])   addFace(FRONT, x, y, z, (uint32_t)layerFor(solid[x+1][y+1][z+1], FRONT), solid, vertices);
        if (!solid[x+1][y+1][z+2]) addFace(BACK,  x, y, z, (uint32_t)layerFor(solid[x+1][y+1][z+1], BACK), solid, vertices);
    }
}

ChunkLoader::~ChunkLoader() {
    stop();
}

void ChunkLoader::start(World& world, unsigned int threadCount) {
    this->world = &world;
    if (threadCount == 0) { // auto
        unsigned int hw = std::thread::hardware_concurrency();
        threadCount = hw > 3 ? hw - 2 : 1;
    }
    running = true;
    for (unsigned int i = 0; i < threadCount; i++) {
        workers.emplace_back(&ChunkLoader::workerLoop, this);
    }

    std::printf("Started ChunkLoader with thread count: %u", threadCount);
}

void ChunkLoader::stop() {
    {
        std::lock_guard lock(dataMutex);
        running = false;
    }
    wake.notify_all();
    for (auto& t : workers) if (t.joinable()) t.join();
    workers.clear();
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

    wake.notify_one();
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
        if ((!chunk.dirty && !chunk.neighborDirty) || remeshPending.count(coord)) continue;
        (chunk.dirty ? toRemesh : toRemeshLow).push(coord); // dirty is player edit
        chunk.dirty = chunk.neighborDirty = false;
        remeshPending.insert(coord);
        wake.notify_one();
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

        if (!job.vertices.empty()) {
            for (int dx = -1; dx <= 1; dx++)
                for (int dy = -1; dy <= 1; dy++)
                    for (int dz = -1; dz <= 1; dz++) {
                        if (!dx && !dy && !dz) continue;
                        auto it = chunks.find(job.coord + glm::ivec3(dx, dy, dz));
                        if (it != chunks.end() && it->second.vertexCount > 0) it->second.neighborDirty = true;
                    }
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

void ChunkLoader::updateAround(World &world, const glm::ivec3 &playerChunk, int renderDistance) {
    const int half = (renderDistance + 1) / 2;
    const int vertHalf = (half + 1) / 2;
    {
        std::lock_guard lock(dataMutex);
        this->playerChunk = playerChunk;

        std::vector<ChunkLoadJob> jobs;
        while (!toLoad.empty()) {
            jobs.push_back(toLoad.top());
            toLoad.pop();
        };

        for (auto& j : jobs) {
            const glm::ivec3 d = j.coord - playerChunk;

            if (d.x < -half - 1 || d.x > half || d.z < -half - 1 || d.z > half ||
                d.y < -vertHalf - 1 || d.y > vertHalf) {
                pending.erase(j.coord);
                continue;
                }
            j.priority = (float)(d.x * d.x + d.y * d.y + d.z * d.z);
            toLoad.push(j);
        }
    }

    requestUnload(world.getChunksToUnload(playerChunk, renderDistance));
    for (int x = -half; x < half; x++)
        for (int y = -vertHalf; y < vertHalf; y++)
            for (int z = -half; z < half; z++) {
                const glm::ivec3 coord = playerChunk + glm::ivec3(x, y, z);
                if (!world.hasChunk(coord)) requestChunk(coord);
            }
}

void ChunkLoader::workerLoop() {
    while (true) {
        ChunkLoadJob job;
        glm::ivec3 remeshCoord{0};
        bool isRemesh = false;
        bool cancelled = false;

        {
            std::unique_lock lock(dataMutex);
            wake.wait(lock, [&] { return !running || !toRemesh.empty() || !toLoad.empty() || !toRemeshLow.empty(); });
            if (!running) return;

            if (!toRemesh.empty()) {
                remeshCoord = toRemesh.front();
                toRemesh.pop();
                isRemesh = true;
            } else if (!toLoad.empty()) {
                job = toLoad.top();
                toLoad.pop();
                cancelled = pendingUnload.count(job.coord) > 0;
            } else {
                remeshCoord = toRemeshLow.front();
                toRemeshLow.pop();
                isRemesh = true;
            }
        }

        if (isRemesh) {
            uint16_t solid[18][18][18];
            std::vector<uint32_t> vertices;
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

        uint16_t solid[18][18][18];
        gatherSolid(job.coord, &chunk, solid);
        std::vector<uint32_t> vertices;
        meshFromSolid(solid, vertices);

        std::lock_guard lock(dataMutex);
        ready.push({job.coord, std::move(chunk), std::move(vertices)});
    }
}
