#include "Player.h"

#include "../util/utils.h"

Player::Player() : camera() {
}

void Player::update(float delta) {
    camera.updateFrustum();
    camera.update(position + glm::vec3(0.f, EYE_HEIGHT, 0.f));
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
    camera.target = normalize(camera.target);
    camera.right = normalize(cross(camera.target, glm::vec3(0.0f, 1.0f, 0.0f)));
    camera.up = normalize(cross(camera.right, camera.target));
}

void Player::move(World& world, glm::vec3 input, float delta) {
    delta = std::min(delta, 0.05f);

    auto& cam = getCamera();
    const glm::vec3 forward = normalize(glm::vec3(cam.target.x, 0.0f, cam.target.z));
    glm::vec3 wish = forward * input.z + cam.right * input.x;
    wish.y = 0.0f;
    if (dot(wish, wish) > 1e-6f) wish = normalize(wish);

    const float t = 1.0f - std::exp(-ACCEL * delta);
    velocity.x += (wish.x * speed - velocity.x) * t;
    velocity.z += (wish.z * speed - velocity.z) * t;

    velocity.y = std::max(velocity.y + GRAVITY * delta, TERMINAL);
    if (input.y > 0.0f && onGround) {
        velocity.y = JUMP_SPEED;
        onGround = false;
    }

    const glm::vec3 total = velocity * delta;
    const float maxMove = std::max({std::abs(total.x), std::abs(total.y), std::abs(total.z)});
    const int steps = std::max(1, int(std::ceil(maxMove / 0.4f)));
    const glm::vec3 stepMove = total / float(steps);

    const float eps = 1e-3f;
    bool groundedThisFrame = false;

    for (int s = 0; s < steps; ++s) {
        for (int axis : {1, 0, 2}) {
            const float d = stepMove[axis];
            if (d == 0.0f) continue;

            glm::vec3 next = position;
            next[axis] += d;

            if (!collides(world, next)) {
                position = next;
                continue;
            }

            const float shift = next[axis] + BLOCK_SHIFT;
            if (axis == 1) {
                if (d < 0.0f) {
                    position.y = std::floor(shift) + 1.0f - BLOCK_SHIFT + eps / 10.0f;
                    groundedThisFrame = true;
                } else {
                    position.y = std::floor(shift + HEIGHT) - HEIGHT - BLOCK_SHIFT - eps; // bump head
                }
            } else {
                if (d > 0.0f) position[axis] = std::floor(shift + HALF_WIDTH) - HALF_WIDTH - BLOCK_SHIFT - eps;
                else position[axis] = std::floor(shift - HALF_WIDTH) + 1.0f + HALF_WIDTH - BLOCK_SHIFT + eps;
            }
            velocity[axis] = 0.0f;
        }
    }
    onGround = groundedThisFrame;
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

bool Player::collides(const World& world, const glm::vec3& pos) const {
    const float eps = 1e-4f;
    const glm::vec3 p = pos + BLOCK_SHIFT;
    const glm::ivec3 lo = glm::ivec3(floor(p + glm::vec3(-HALF_WIDTH, 0.0f,   -HALF_WIDTH)));
    const glm::ivec3 hi = glm::ivec3(floor(p + glm::vec3( HALF_WIDTH - eps, HEIGHT - eps, HALF_WIDTH - eps)));

    for (int x = lo.x; x <= hi.x; ++x)
        for (int y = lo.y; y <= hi.y; ++y)
            for (int z = lo.z; z <= hi.z; ++z) {
                const glm::ivec3 block(x, y, z);
                if (!world.hasChunk(worldToChunkPos(block))) return true;
                if (world.getBlock(block) != 0) return true;
            }
    return false;
}
