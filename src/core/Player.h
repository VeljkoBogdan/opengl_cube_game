#ifndef PLAYER_H
#define PLAYER_H
#include "camera.h"
#include "world.h"

class Player {
public:
    Player();
    void update(float delta);
    void mouseCallback(double xPos, double yPos);
    void move(World& world, glm::vec3 input, float delta);
    Camera& getCamera();
    [[nodiscard]] int getRenderDistance() const;
    [[nodiscard]] float getSpeed() const;
    bool collides(const World& world, const glm::vec3& pos) const;
    glm::vec3 position{0.0f};
private:
    bool onGround = false;
    Camera camera;
    int renderDistance = 16;
    glm::vec3 velocity{0.0f};

    bool firstMouse = true; // used for smoothing the first mouse movement
    float lastMouseX = 400.0f;
    float lastMouseY = 300.0f;
    float mouseSensitivity = 0.1f;
    float yaw = -90.0f;
    float pitch = 0.0f;

    float speed = 8.0f;
    static constexpr float HALF_WIDTH = 0.3f;
    static constexpr float HEIGHT = 1.8f;
    static constexpr float EYE_HEIGHT = 1.62f;
    static constexpr float GRAVITY = -28.0f;
    static constexpr float JUMP_SPEED = 9.0f;
    static constexpr float TERMINAL = -50.0f;
    static constexpr float ACCEL = 12.0f;
    static constexpr float BLOCK_SHIFT = 0.5f;
};

#endif //PLAYER_H
