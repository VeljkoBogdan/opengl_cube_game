#include "camera.h"

glm::mat4 Camera::getViewMatrix() const {
    return glm::lookAt(position, position + target, up);
}

glm::mat4 Camera::getProjectionMatrix() const {
    return glm::perspective(glm::radians(zoom), SCR_RATIO, 0.1f, 1000.f);
}

bool Camera::isChunkVisible(const glm::ivec3 chunkPos) const {
    const glm::vec3 bmin = glm::vec3(chunkPos * CHUNK_WIDTH);
    const glm::vec3 bmax = bmin + glm::vec3(CHUNK_WIDTH * 1.f);

    for (auto p : frustumPlanes) {
        glm::vec3 pos;
        pos.x = p.x >= 0 ? bmax.x : bmin.x;
        pos.y = p.y >= 0 ? bmax.y : bmin.y;
        pos.z = p.z >= 0 ? bmax.z : bmin.z;

        if (glm::dot(glm::vec3(p), pos) + p.w < 0) return false;
    }

    return true;
}

void Camera::updateFrustum() {
    glm::mat4 vp = getProjectionMatrix() * getViewMatrix();

    // Left
    frustumPlanes[0] = glm::vec4(
        vp[0][3] + vp[0][0],
        vp[1][3] + vp[1][0],
        vp[2][3] + vp[2][0],
        vp[3][3] + vp[3][0]
    );
    // Right
    frustumPlanes[1] = glm::vec4(
        vp[0][3] - vp[0][0],
        vp[1][3] - vp[1][0],
        vp[2][3] - vp[2][0],
        vp[3][3] - vp[3][0]
    );
    // Bottom
    frustumPlanes[2] = glm::vec4(
        vp[0][3] + vp[0][1],
        vp[1][3] + vp[1][1],
        vp[2][3] + vp[2][1],
        vp[3][3] + vp[3][1]
    );
    // Top
    frustumPlanes[3] = glm::vec4(
        vp[0][3] - vp[0][1],
        vp[1][3] - vp[1][1],
        vp[2][3] - vp[2][1],
        vp[3][3] - vp[3][1]
    );
    // Near
    frustumPlanes[4] = glm::vec4(
        vp[0][3] + vp[0][2],
        vp[1][3] + vp[1][2],
        vp[2][3] + vp[2][2],
        vp[3][3] + vp[3][2]
    );
    // Far
    frustumPlanes[5] = glm::vec4(
        vp[0][3] - vp[0][2],
        vp[1][3] - vp[1][2],
        vp[2][3] - vp[2][2],
        vp[3][3] - vp[3][2]
    );
}
