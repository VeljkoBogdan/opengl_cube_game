#ifndef BLOCKINTERACTION_H
#define BLOCKINTERACTION_H

#include <optional>

#include "camera.h"
#include "Raycast.h"
#include "world.h"

struct BlockHit {
    glm::ivec3 pos;
    glm::ivec3 normal;
};

inline std::optional<BlockHit> raycastBlock(World& world, const Camera& cam, float reach) {
    Raycast ray(cam.position, cam.position + cam.target * reach);
    while (auto p = ray.getNextTargetPos()) {
        if (world.getBlock(*p) != 0)
            return BlockHit{*p, (glm::ivec3)normalize(ray.getNormal())};
    }
    return std::nullopt;
}

inline void breakBlock(World& world, const BlockHit& hit) {
    world.setBlock(hit.pos, 0);
}

inline void placeBlock(World& world, const BlockHit& hit, int blockId) {
    world.setBlock(hit.pos + hit.normal, blockId);
}

#endif //BLOCKINTERACTION_H
