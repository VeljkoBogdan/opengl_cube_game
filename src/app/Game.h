#ifndef GAME_H
#define GAME_H

#include <glad/glad.h>
#include "Window.h"

#include <optional>
#include <glm/glm.hpp>
#include "GLFW/glfw3.h"
#include "../core/BlockInteraction.h"
#include "../core/chunkLoader.h"
#include "../core/Player.h"
#include "../gl/Framebuffer.h"
#include "../gl/Texture2DArray.h"
#include "../rendering/BlockOutline.h"
#include "../rendering/Crosshair.h"
#include "../rendering/PostProcess.h"
#include "../rendering/Sky.h"
#include "../rendering/Skybox.h"
#include "../rendering/ui/UIRenderer.h"

class Game {
public:
    Game();
    ~Game();
    void run();
private:
    static Game* self(GLFWwindow* window) { return static_cast<Game *>(glfwGetWindowUserPointer(window)); }
    void onKey(int key, int action);
    void onMouseButton(int button, int action);
    void onResize(int w, int h);

    void updateTime();
    void updateChunkStreaming();
    void updateInteraction();
    glm::vec3 readMoveInput() const;
    void renderScene(const SkyState& sky, const std::optional<BlockHit>& target, glm::ivec2 fb);
    void renderUI(glm::ivec2 fb);
    void setChunkUniforms(const SkyState& sky, const glm::mat4& view, const glm::mat4& proj);

    static constexpr float kReach = 6.0f;
    static constexpr float kDayLengthSeconds = 600.0f;

    Window window{1280, 720, "Voxel"};
    World world;
    Player player;
    ChunkLoader chunkLoader;
    ChunkRenderer chunkRenderer{world};
    Framebuffer sceneFbo;
    Texture2DArray blockTextures;
    Shader chunkShader;
    Skybox skybox;
    PostProcess post;
    Crosshair crosshair;
    BlockOutline blockOutline;
    UIRenderer ui;

    float deltaTime = 0.0f, lastTime = 0.0f, timeOfDay = 0.0f;
    glm::ivec3 lastPlayerChunk{INT_MAX};
    bool breakRequested = false, placeRequested = false;
};

#endif //GAME_H
