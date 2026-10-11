#ifndef CHUNK_RENDERER_H
#define CHUNK_RENDERER_H

#include "drawCmd.h"
#include "vertexArena.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "../core/chunk.h"
#include "../core/world.h"
#include "../shader.h"
#include "../core/camera.h"
#include "../gl/GLBuffer.h"
#include "../gl/VertexArray.h"

class ChunkRenderer {
public:
    ChunkRenderer(World& world) : world(world) {}

    // needs a live gl context
    void init();

    // copies the mesh into the shared buffer
    void upload(Chunk& chunk, const std::vector<float>& vertices);
    // returns the chunk's range to the arena
    void release(const Chunk& chunk);

    void drawAll(Shader& shader, Camera& camera);
private:
    World& world;
    GLBuffer geometry{GL_ARRAY_BUFFER};
    GLBuffer offsetBuf{GL_ARRAY_BUFFER};
    GLBuffer indirectBuf{GL_DRAW_INDIRECT_BUFFER};
    VertexArray vao;
    VertexArena arena;
    std::vector<DrawCmd> cmds;
    std::vector<glm::vec3> offsets;
};

#endif