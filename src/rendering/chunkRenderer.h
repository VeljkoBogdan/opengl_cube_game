#ifndef CHUNK_RENDERER_H
#define CHUNK_RENDERER_H

#include <unordered_map>

#include "DrawCmd.h"
#include "VertexArena.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "../core/chunk.h"
#include "../core/world.h"
#include "../shader.h"
#include "../util/IVec3Hash.h"
#include "../constants.h"
#include "../util/direction.h"
#include "../core/camera.h"

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

    GLuint VAO = 0, VBO = 0;
    GLuint offsetVBO = 0, indirectBuf = 0;
    VertexArena arena;

    std::vector<DrawCmd> cmds;
    std::vector<glm::vec3> offsets;
};

#endif