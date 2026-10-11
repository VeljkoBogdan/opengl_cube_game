#include "chunkRenderer.h"

constexpr uint32_t MAX_VERTS = 16'000'000;
constexpr int FLOATS_PER_VERTEX = 8; // pos(3) normal(3) uv(2)
constexpr size_t VERTEX_BYTES = FLOATS_PER_VERTEX * sizeof(float);
constexpr int CHUNK_SIZE_BLOCKS = 16;

void ChunkRenderer::init() {
    geometry.allocate((size_t)MAX_VERTS * VERTEX_BYTES);
    vao.addBuffer(geometry, VertexLayout().floats(3).floats(3).floats(2));
    vao.addBuffer(offsetBuf, VertexLayout().floats(3).perInstance());

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

    geometry.update((size_t)first * VERTEX_BYTES, vertices.data(), vertices.size() * sizeof(float));

    chunk.firstVertex = (int)first;
    chunk.vertexCount = (int)count;
}

void ChunkRenderer::release(const Chunk &chunk) {
    if (chunk.vertexCount > 0) {
        arena.free((uint32_t)chunk.firstVertex, (uint32_t)chunk.vertexCount);
    }
}

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

    offsetBuf.setData(offsets, GL_STREAM_DRAW);
    indirectBuf.setData(cmds, GL_STREAM_DRAW);

    vao.bind();
    glMultiDrawArraysIndirect(GL_TRIANGLES, nullptr, (GLsizei)cmds.size(), 0);
}
