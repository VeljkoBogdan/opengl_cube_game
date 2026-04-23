#include "chunkRenderer.h"

void ChunkRenderer::buildMesh(Chunk &chunk, glm::ivec3 chunkPos) {
    std::vector<float> vertices;

    auto getBlock = [&](int x, int y, int z) -> int {
        if (x >= 0 && x < 16 &&
            y >= 0 && y < 16 &&
            z >= 0 && z < 16)
            return chunk.blocks[x][y][z];

        glm::ivec3 neighborPos = chunkPos;
        int nx = x, ny = y, nz = z;

        if (x < 0)  { neighborPos.x--; nx = 15; }
        if (x >= 16){ neighborPos.x++; nx = 0;  }
        if (y < 0)  { neighborPos.y--; ny = 15; }
        if (y >= 16){ neighborPos.y++; ny = 0;  }
        if (z < 0)  { neighborPos.z--; nz = 15; }
        if (z >= 16){ neighborPos.z++; nz = 0;  }

        auto it = world.getChunks().find(neighborPos);
        if (it == world.getChunks().end()) return 0; // no neighbor, treat as air
        return it->second.blocks[nx][ny][nz];
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
