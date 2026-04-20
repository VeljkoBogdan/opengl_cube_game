#include "camera.h"

glm::mat4 Camera::getViewMatrix() {
    return glm::lookAt(position, position + target, up);
}
