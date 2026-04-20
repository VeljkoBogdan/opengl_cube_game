#ifndef IVEC3HASH_H
#define IVEC3HASH_H

#include <cstddef>
#include <functional>
#include "glm/glm.hpp"

struct IVec3Hash {
    size_t operator()(const glm::ivec3& v) const {
        size_t h1 = std::hash<int>()(v.x);
        size_t h2 = std::hash<int>()(v.y);
        size_t h3 = std::hash<int>()(v.z);
        return h1 ^ (h2 << 16) ^ (h3 << 32);
    }
};

#endif