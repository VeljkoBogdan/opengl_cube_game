#ifndef PLAYER_H
#define PLAYER_H
#include "camera.h"

class Player {
public:
    Player();
    void update(float delta);
    void mouseCallback(double xPos, double yPos);
    Camera& getCamera();
    [[nodiscard]] int getRenderDistance() const;
    [[nodiscard]] float getSpeed() const;
private:
    Camera camera;
    int renderDistance = 8;
    float speed = 32.0f;

    bool firstMouse = true; // used for smoothing the first mouse movement
    float lastMouseX = 400.0f;
    float lastMouseY = 300.0f;
    float mouseSensitivity = 0.1f;
    float yaw = -90.0f;
    float pitch = 0.0f;
};

#endif //PLAYER_H
