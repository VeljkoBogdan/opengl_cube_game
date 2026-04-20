#include "chunkRenderer.h"

void ChunkRenderer::buildMesh(Chunk &chunk, glm::ivec3 chunkPos) {
    std::vector<float> vertices;

    for (int x = 0; x < CHUNK_WIDTH; x++)
    for (int y = 0; y < CHUNK_HEIGHT; y++)
    for (int z = 0; z < CHUNK_LENGTH; z++) {
        if (chunk.blocks[x][y][z] == 0) continue;

        if (y == 0 || chunk.blocks[x][y-1][z] == 0) addFace(Direction::DOWN, x, y, z, vertices);
        if (y == CHUNK_HEIGHT-1 || chunk.blocks[x][y+1][z] == 0) addFace(Direction::UP, x, y, z, vertices);
        if (x == 0 || chunk.blocks[x-1][y][z] == 0) addFace(Direction::LEFT, x, y, z, vertices);
        if (x == CHUNK_WIDTH-1 || chunk.blocks[x+1][y][z] == 0) addFace(Direction::RIGHT, x, y, z, vertices);
        if (z == 0 || chunk.blocks[x][y][z+1] == 0) addFace(Direction::FRONT, x, y, z, vertices);
        if (z == CHUNK_LENGTH-1 || chunk.blocks[x][y][z-1] == 0) addFace(Direction::BACK, x, y, z, vertices);
    }

    glBindVertexArray(chunk.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, chunk.VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    chunk.vertexCount = vertices.size() / 6; // depends on the vertex layout
}

void ChunkRenderer::draw(Chunk &chunk, Shader &shader, glm::ivec3 coords) {
    glBindVertexArray(chunk.VAO);
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(coords * 16));
    shader.setMat4("u_model", model);
    glDrawArrays(GL_TRIANGLES, 0, chunk.vertexCount);
}

void ChunkRenderer::addFace(int face, int x, int y, int z, std::vector<float>& verticesStorage) {
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
            x-0.5f, y-0.5f, z+0.5f,  0.0f, 0.0f, -1.0f,
            x+0.5f, y-0.5f, z+0.5f,  0.0f, 0.0f, -1.0f,
            x-0.5f, y+0.5f, z+0.5f,  0.0f, 0.0f, -1.0f,
            x-0.5f, y+0.5f, z+0.5f,  0.0f, 0.0f, -1.0f,
            x+0.5f, y-0.5f, z+0.5f,  0.0f, 0.0f, -1.0f,
            x+0.5f, y+0.5f, z+0.5f,  0.0f, 0.0f, -1.0f
        });
        break;
    case Direction::FRONT:
        verticesStorage.insert(verticesStorage.end(), {
            x-0.5f, y-0.5f, z-0.5f,  0.0f, 0.0f, 1.0f,
            x+0.5f, y+0.5f, z-0.5f,  0.0f, 0.0f, 1.0f,
            x+0.5f, y-0.5f, z-0.5f,  0.0f, 0.0f, 1.0f,
            x-0.5f, y-0.5f, z-0.5f,  0.0f, 0.0f, 1.0f,
            x-0.5f, y+0.5f, z-0.5f,  0.0f, 0.0f, 1.0f,
            x+0.5f, y+0.5f, z-0.5f,  0.0f, 0.0f, 1.0f
        });
        break;
    default:
        break;
    }
}
