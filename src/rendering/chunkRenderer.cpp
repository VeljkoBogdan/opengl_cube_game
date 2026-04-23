#include "chunkRenderer.h"

void ChunkRenderer::draw(Chunk &chunk, Shader &shader, glm::ivec3 coords) {
    if (chunk.vertexCount == 0) return; // empty chunk
    glBindVertexArray(chunk.VAO);
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(coords * 16));
    shader.setMat4("u_model", model);
    glDrawArrays(GL_TRIANGLES, 0, chunk.vertexCount);
}

void ChunkRenderer::drawAll(Shader &shader) {
    for (auto& [coords, chunk] : world.getChunks()) {
        if (chunk.vertexCount <= 0) continue; // empty chunk
        glBindVertexArray(chunk.VAO);
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(coords * 16));
        shader.setMat4("u_model", model);
        glDrawArrays(GL_TRIANGLES, 0, chunk.vertexCount);
    }
}
