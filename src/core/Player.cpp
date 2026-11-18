#include "Player.h"

Player::Player() : camera() {
}

void Player::update(float delta) {
    camera.updateFrustum();
}

void Player::mouseCallback(double xPos, double yPos) {
    if (firstMouse) {
        lastMouseX = xPos;
        lastMouseY = yPos;
        firstMouse = false;
    }

    float x = (xPos - lastMouseX) * mouseSensitivity;
    float y = (lastMouseY - yPos) * mouseSensitivity;
    lastMouseX = xPos;
    lastMouseY = yPos;

    yaw += x;
    pitch += y;

    if (pitch > 89.9f) pitch = 89.9f;
    if (pitch < -89.9f) pitch = -89.9f;

    camera.target.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
    camera.target.y = sin(glm::radians(pitch));
    camera.target.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
    camera.target = glm::normalize(camera.target);
    camera.right = glm::normalize(glm::cross(camera.target, glm::vec3(0.0f, 1.0f, 0.0f)));
    camera.up = glm::normalize(glm::cross(camera.right, camera.target));
}

void Player::move(glm::vec3 input, float delta) {
    if (dot(input, input) == 0.0f) return;
    input = normalize(input);
    Camera& cam = getCamera();
    const glm::vec3 forward = glm::normalize(glm::vec3(cam.target.x, 0.0f, cam.target.z));
    cam.position += getSpeed() * delta * (forward * input.z + cam.right * input.x + glm::vec3(0, 1, 0) * input.y);
}

Camera& Player::getCamera() {
    return camera;
}

int Player::getRenderDistance() const {
    return renderDistance;
}

float Player::getSpeed() const {
    return speed;
}
