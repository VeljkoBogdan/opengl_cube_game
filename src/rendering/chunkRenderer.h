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

    void buildMesh(Chunk& chunk, glm::ivec3 chunkPos);
    void draw(Chunk& chunk, Shader& shader, glm::ivec3 coords);
    void drawAll(Shader& shader);
private:
    World& world;

    void addFace(int face, int x, int y, int z, std::vector<float>& verticesStorage);
    bool isSolid(int blockId);
};

#endif