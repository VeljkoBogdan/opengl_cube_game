//
// Created by pc on 02/11/2026.
//

#include "Raycast.h"

#include <optional>

#include "glm/ext/quaternion_geometric.hpp"

Raycast::Raycast(glm::vec3 from, glm::vec3 to, Context raycastContext) {
    this->from = from;
    this->to = to;
    this->raycastContext = raycastContext;

    glm::vec3 shiftedOrigin = from + glm::vec3(0.5f, 0.5f, 0.5f);
    glm::vec3 dir = to - from;

    pos = glm::ivec3(glm::floor(shiftedOrigin));

    stepX = dir.x < 0 ? -1 : 1;
    stepY = dir.y < 0 ? -1 : 1;
    stepZ = dir.z < 0 ? -1 : 1;

    deltaX = std::abs(1.0 / dir.x);
    deltaY = std::abs(1.0 / dir.y);
    deltaZ = std::abs(1.0 / dir.z);

    tmaxX = (dir.x < 0 ? shiftedOrigin.x - pos.x : (pos.x + 1.0) - shiftedOrigin.x) * deltaX;
    tmaxY = (dir.y < 0 ? shiftedOrigin.y - pos.y : (pos.y + 1.0) - shiftedOrigin.y) * deltaY;
    tmaxZ = (dir.z < 0 ? shiftedOrigin.z - pos.z : (pos.z + 1.0) - shiftedOrigin.z) * deltaZ;
}

std::optional<glm::ivec3> Raycast::getNextTargetPos() {
    normal = glm::vec3(0);
    if (steps == 0) {
        steps++;
        return pos;
    }

    steps++;
    if (tmaxX < tmaxY) {
        if (tmaxX < tmaxZ) {
            // tmaxX smallest
            if (tmaxX > 1.0) return std::nullopt;
            pos.x += stepX;
            tmaxX += deltaX;
            normal = glm::vec3(-stepX, 0, 0);
        } else {
            // tmaxZ smallest
            if (tmaxZ > 1.0) return std::nullopt;
            pos.z += stepZ;
            tmaxZ += deltaZ;
            normal = glm::vec3(0, 0, -stepZ);
        }
    } else {
        if (tmaxY < tmaxZ) {
            // tmaxY smallest
            if (tmaxY > 1.0) return std::nullopt;
            pos.y += stepY;
            tmaxY += deltaY;
            normal = glm::vec3(0, -stepY, 0);
        } else {
            // tmaxZ smallest
            if (tmaxZ > 1.0) return std::nullopt;
            pos.z += stepZ;
            tmaxZ += deltaZ;
            normal = glm::vec3(0, 0, -stepZ);
        }
    }

    return pos;
}

glm::vec3 Raycast::getNormal() {
    return normal;
}
