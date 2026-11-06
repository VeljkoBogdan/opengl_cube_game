#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "chunkLoader.h"
#include "../util/direction.h"

constexpr int chunksUploadedPerFrame = 8;
constexpr int chunksUnloadedPerFrame = 8;

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

void ChunkLoader::addFace(int face, int x, int y, int z, float layer,
                          const uint16_t (&ids)[18][18][18], std::vector<float>& verticesStorage) {
    constexpr float h = 0.5f;
    const FaceDef f = faceDef(face);

    const glm::ivec3 front = glm::ivec3(x + 1, y + 1, z + 1) + f.n;
    auto occ = [&](glm::ivec3 p) { return ids[p.x][p.y][p.z] != 0 ? 1 : 0; };

    static const int su[4] = {-1,  1, 1, -1};
    static const int sv[4] = {-1, -1, 1,  1};
    static const float uv[4][2] = {{0, 0}, {1, 0}, {1, 1}, {0, 1}};

    glm::vec3 pos[4];
    float ao[4];
    for (int i = 0; i < 4; i++) {
        glm::ivec3 ts = f.t * su[i], bs = f.b * sv[i];
        int s1 = occ(front + ts);
        int s2 = occ(front + bs);
        int c  = occ(front + ts + bs);
        ao[i] = (s1 && s2) ? 0.0f : float(3 - (s1 + s2 + c));

        pos[i] = glm::vec3(x, y, z) + glm::vec3(f.n) * h
               + glm::vec3(f.t) * (su[i] * h) + glm::vec3(f.b) * (sv[i] * h);
    }

    static const int tris[2][6] = {{0, 1, 2, 0, 2, 3}, {1, 2, 3, 1, 3, 0}};
    const int* order = (ao[0] + ao[2] > ao[1] + ao[3]) ? tris[0] : tris[1];

    for (int k = 0; k < 6; k++) {
        int i = order[k];
        verticesStorage.insert(verticesStorage.end(), {
            pos[i].x, pos[i].y, pos[i].z,
            (float)f.n.x, (float)f.n.y, (float)f.n.z,
            uv[i][0], uv[i][1],
            layer, ao[i]
        });
    }
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
            for (int z = 0; z < CHUNK_LENGTH; z++)
                solid[x+1][y+1][z+1] = self->blocks[x][y][z];

    Chunk* nb[3][3][3] = {};
    for (int dx = -1; dx <= 1; dx++)
        for (int dy = -1; dy <= 1; dy++)
            for (int dz = -1; dz <= 1; dz++) {
                if (!dx && !dy && !dz) continue;
                auto it = chunks.find(coord + glm::ivec3(dx, dy, dz));
                if (it != chunks.end()) nb[dx+1][dy+1][dz+1] = &it->second;
            }

    for (int px = 0; px < 18; px++)
        for (int py = 0; py < 18; py++)
            for (int pz = 0; pz < 18; pz++) {
                int dx = px == 0 ? -1 : (px == 17 ? 1 : 0);
                int dy = py == 0 ? -1 : (py == 17 ? 1 : 0);
                int dz = pz == 0 ? -1 : (pz == 17 ? 1 : 0);
                if (!dx && !dy && !dz) continue;
                Chunk* c = nb[dx+1][dy+1][dz+1];
                if (c) solid[px][py][pz] = c->blocks[(px + 15) % 16][(py + 15) % 16][(pz + 15) % 16];
            }
    return true;
}

void ChunkLoader::meshFromSolid(const uint16_t(&solid)[18][18][18], std::vector<float> &vertices) {
    for (int x = 0; x < CHUNK_WIDTH; x++)
    for (int y = 0; y < CHUNK_HEIGHT; y++)
    for (int z = 0; z < CHUNK_LENGTH; z++) {
        if (!solid[x+1][y+1][z+1]) continue;

        if (!solid[x+1][y][z+1])   addFace(DOWN,  x, y, z, (float)layerFor(solid[x+1][y+1][z+1], DOWN), solid, vertices);
        if (!solid[x+1][y+2][z+1]) addFace(UP,    x, y, z, (float)layerFor(solid[x+1][y+1][z+1], UP), solid, vertices);
        if (!solid[x][y+1][z+1])   addFace(LEFT,  x, y, z, (float)layerFor(solid[x+1][y+1][z+1], LEFT), solid, vertices);
        if (!solid[x+2][y+1][z+1]) addFace(RIGHT, x, y, z, (float)layerFor(solid[x+1][y+1][z+1], RIGHT), solid, vertices);
        if (!solid[x+1][y+1][z])   addFace(FRONT, x, y, z, (float)layerFor(solid[x+1][y+1][z+1], FRONT), solid, vertices);
        if (!solid[x+1][y+1][z+2]) addFace(BACK,  x, y, z, (float)layerFor(solid[x+1][y+1][z+1], BACK), solid, vertices);
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
        if (!chunk.dirty || remeshPending.count(coord)) continue;
        chunk.dirty = false;
        remeshPending.insert(coord);
        toRemesh.push(coord);

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

        for (int dx = -1; dx <= 1; dx++)
            for (int dy = -1; dy <= 1; dy++)
                for (int dz = -1; dz <= 1; dz++) {
                    if (!dx && !dy && !dz) continue;
                    auto it = chunks.find(job.coord + glm::ivec3(dx, dy, dz));
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
    while (true) {
        ChunkLoadJob job;
        glm::ivec3 remeshCoord{0};
        bool isRemesh = false;
        bool cancelled = false;

        {
            std::unique_lock lock(dataMutex);
            wake.wait(lock, [&] { return !running || !toRemesh.empty() || !toLoad.empty(); });
            if (!running) return;

            if (!toRemesh.empty()) {
                remeshCoord = toRemesh.front();
                toRemesh.pop();
                isRemesh = true;
            } else {
                job = toLoad.top();
                toLoad.pop();
                cancelled = pendingUnload.count(job.coord) > 0;
            }
        }

        if (isRemesh) {
            uint16_t solid[18][18][18];
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

        uint16_t solid[18][18][18];
        gatherSolid(job.coord, &chunk, solid);
        std::vector<float> vertices;
        meshFromSolid(solid, vertices);

        std::lock_guard lock(dataMutex);
        ready.push({job.coord, std::move(chunk), std::move(vertices)});
    }
}