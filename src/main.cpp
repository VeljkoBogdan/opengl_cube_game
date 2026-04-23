#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include "util/stb_image.h"

#include <iostream>
#include <cmath>
#include <unordered_map>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "shader.h"
#include "core/camera.h"
#include "core/chunk.h"
#include "core/world.h"
#include "core/worldGenerator.h"
#include "core/chunkLoader.h"
#include "rendering/chunkRenderer.h"
#include "util/IVec3Hash.h"
#include "util/utils.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow* window, double xPos, double yPos);
void scroll_callback(GLFWwindow* window, double xOffset, double yOffset);
void find_spawn(World& world, Camera& camera);

// settings
const unsigned int SCR_WIDTH = 1240;
const unsigned int SCR_HEIGHT = 800;

float deltaTime = 0.0f;
float lastTime = 0.0f;
int nbFrames = 0;

// mouse
bool firstMouse = true; // used for smoothing the first mouse movement
float lastMouseX = 400.0f;
float lastMouseY = 300.0f;
float mouseSensitivity = 0.1f;
float yaw = -90.0f;
float pitch = 0.0f;

// camera
Camera camera;
float speed = 60.0f;
int renderDistance = 16;

// world
World world;
ChunkRenderer chunkRenderer(world);
ChunkLoader chunkLoader;

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // 1 = vsync, 0 = uncapped
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }    

    Shader shader(
        "C:/Users/pc/Documents/opengl_projects/opengl_cube_game/resources/shaders/vertex.vert",
        "C:/Users/pc/Documents/opengl_projects/opengl_cube_game/resources/shaders/fragment.frag"
    );

    double lastFPSTime = glfwGetTime();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    find_spawn(world, camera);

    chunkLoader.start();

    glm::ivec3 lastPlayerChunk = worldToChunkPos(camera.position);

    // Loop
    while (!glfwWindowShouldClose(window)) {
        // getting delta time
        float currentTime = glfwGetTime();
        deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        // fps
        double currentFPSTime = glfwGetTime();
        nbFrames++;
        if (currentFPSTime - lastFPSTime >= 1.0) {
            printf("%f ms/frame\n", 1000.0/double(nbFrames));
            nbFrames = 0;
            lastFPSTime = currentFPSTime;
        }

        glm::ivec3 currentPlayerChunk = worldToChunkPos(camera.position);
        if (currentPlayerChunk != lastPlayerChunk) {
            auto chunksToUnload = world.getChunksToUnload(currentPlayerChunk, renderDistance);
            world.unloadChunks(chunksToUnload);
            chunkLoader.unloadChunks(chunksToUnload);
            lastPlayerChunk = currentPlayerChunk;
        }

        // requesting chunks
        int half = renderDistance / 2;
        for (int x = -half; x < half; x++)
        for (int y = -half; y < half; y++)
        for (int z = -half; z < half; z++) {
            glm::ivec3 coord = worldToChunkPos(camera.position) + glm::ivec3(x, y, z);
            if (!world.hasChunk(coord)) chunkLoader.requestChunk(coord);
        }

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // model view and projection
        glm::mat4 model, view, proj;
        model = view = proj = glm::mat4(1.0f);
        view = camera.getViewMatrix();
        proj = glm::perspective(glm::radians(camera.zoom), (float)SCR_WIDTH/(float)SCR_HEIGHT, 0.1f, 1000.0f);

        // setting uniforms
        shader.use();
        shader.setMat4("u_view", view);
        shader.setMat4("u_proj", proj);
        shader.setVec3("u_ambient", glm::vec3(0.04f, 0.04f, 0.05f));
        shader.setVec3("u_diffuse", glm::vec3(0.75f, 0.75f, 0.7f));
        shader.setVec3("u_lightPos", glm::vec3(128.0f));

        chunkLoader.processReady(world);

        chunkRenderer.drawAll(shader);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    chunkLoader.stop();

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window) {
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) camera.position += speed * deltaTime * glm::normalize(glm::vec3(camera.target.x, 0.0f, camera.target.z));
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) camera.position -= speed * deltaTime * glm::normalize(glm::vec3(camera.target.x, 0.0f, camera.target.z));
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) camera.position -= speed * deltaTime * camera.right;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) camera.position += speed * deltaTime * camera.right;

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) camera.position += speed * deltaTime * glm::vec3(0.0f, 1.0f, 0.0f);
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) camera.position -= speed * deltaTime * glm::vec3(0.0f, 1.0f, 0.0f);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xPos, double yPos) {
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

void scroll_callback(GLFWwindow* window, double xOffset, double yOffset) {
    
}

void find_spawn(World& world, Camera& camera) {
    // int above = 512, below = -256;

    // // find upper limit
    // for (int y = above; y > below; y--) {
    //     if (world.getBlock(glm::ivec3(0, y, 0)) != 0) {
    //         above = y + 2;
    //         break;
    //     }
    // }

    // camera.position = glm::vec3(0.0f, (float)above, 0.0f);

    camera.position = glm::vec3(0.0f, 80.0f, 0.0f);
}