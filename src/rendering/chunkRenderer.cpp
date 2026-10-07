#include "chunkRenderer.h"

constexpr uint32_t MAX_VERTS = 64'000'000; // 32B * 64'000'000 ~2GB
constexpr int FLOATS_PER_VERTEX = 8; // pos(3) normal(3) uv(2)
constexpr size_t VERTEX_BYTES = FLOATS_PER_VERTEX * sizeof(float);
constexpr int CHUNK_SIZE_BLOCKS = 16;

void ChunkRenderer::drawAll(Shader &shader, Camera& camera) {
    (void)shader; // view/proj are set once per frame in main
    cmds.clear();
    offsets.clear();

    for (auto& [coords, chunk] : world.getChunks()) {
        if (chunk.vertexCount <= 0 || !camera.isChunkVisible(coords)) continue;
        cmds.push_back({ (GLuint)chunk.vertexCount, 1u, (GLuint)chunk.firstVertex,
            (GLuint)offsets.size() });
        offsets.push_back(glm::vec3(coords * CHUNK_SIZE_BLOCKS));
    }

    if (cmds.empty()) return;

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, offsetVBO);
    glBufferData(GL_ARRAY_BUFFER, offsets.size() * sizeof(glm::vec3), offsets.data(), GL_STREAM_DRAW);

    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indirectBuf);
    glBufferData(GL_DRAW_INDIRECT_BUFFER, cmds.size() * sizeof(DrawCmd), cmds.data(), GL_STREAM_DRAW);

    glMultiDrawArraysIndirect(GL_TRIANGLES, nullptr, (GLsizei)cmds.size(), 0);
}

void ChunkRenderer::init() {
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // shared geo buffer
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)(MAX_VERTS * VERTEX_BYTES), nullptr, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTES, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTES, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, VERTEX_BYTES, (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // perchunk world offset
    glGenBuffers(1, &offsetVBO);
    glBindBuffer(GL_ARRAY_BUFFER, offsetVBO);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(3);
    glVertexAttribDivisor(3, 1);

    glGenBuffers(1, &indirectBuf);

    arena = VertexArena(MAX_VERTS);
}

void ChunkRenderer::upload(Chunk &chunk, const std::vector<float> &vertices) {
    chunk.vertexCount = 0;
    uint32_t count = (uint32_t)(vertices.size() / FLOATS_PER_VERTEX);
    if (count == 0) return;

    uint32_t first;
    if (!arena.alloc(count, first)) {
        std::cerr << "Vertex arena full, chunk skipped" << std::endl;
        return;
    }

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER, (GLintptr)((size_t)first * VERTEX_BYTES),
        vertices.size() * sizeof(float), vertices.data());

    chunk.firstVertex = (int)first;
    chunk.vertexCount = (int)count;
}

void ChunkRenderer::release(const Chunk &chunk) {
    if (chunk.vertexCount > 0) {
        arena.free((uint32_t)chunk.firstVertex, (uint32_t)chunk.vertexCount);
    }
}
