#ifndef UTILS_H
#define UTILS_H

#include "glm/glm.hpp"
#include "../constants.h"

static constexpr glm::ivec3 worldToChunkPos(float x, float y, float z) {
    return glm::ivec3(
        glm::floor(x / CHUNK_WIDTH),
        glm::floor(y / CHUNK_HEIGHT),
        glm::floor(z / CHUNK_LENGTH)
    );
}

static constexpr glm::ivec3 worldToChunkPos(glm::vec3 pos) {
    return glm::ivec3(
        glm::floor(pos.x / CHUNK_WIDTH),
        glm::floor(pos.y / CHUNK_HEIGHT),
        glm::floor(pos.z / CHUNK_LENGTH)
    );
}

#endif