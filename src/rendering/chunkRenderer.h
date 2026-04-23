#ifndef CHUNK_RENDERER_H
#define CHUNK_RENDERER_H

#include <unordered_map>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "../core/chunk.h"
#include "../core/world.h"
#include "../shader.h"
#include "../util/IVec3Hash.h"
#include "../constants.h"
#include "../util/direction.h"

class ChunkRenderer {
public:
    ChunkRenderer(World& world) : world(world) {}

    void draw(Chunk& chunk, Shader& shader, glm::ivec3 coords);
    void drawAll(Shader& shader);
private:
    World& world;
};

#endif