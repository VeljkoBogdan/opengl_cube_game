#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "chunk.h"
#include "../constants.h"

class Camera {
public:
    glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 target = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 right = glm::vec3(1.0f, 0.0f, 0.0f);
    glm::vec4 frustumPlanes[6];
    glm::mat4 getViewMatrix() const;
    glm::mat4 getProjectionMatrix() const;
    bool isChunkVisible(glm::ivec3 chunkPos) const;
    void updateFrustum();

    float zoom = 90.0f;
};

#endif