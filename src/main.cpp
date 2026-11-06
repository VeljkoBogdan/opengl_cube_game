#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include <iostream>
#include <cmath>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "shader.h"
#include "core/camera.h"
#include "core/world.h"
#include "core/worldGenerator.h"
#include "core/chunkLoader.h"
#include "core/Raycast.h"
#include "gl/Framebuffer.h"
#include "gl/Texture2D.h"
#include "gl/Texture2DArray.h"
#include "rendering/BlockOutline.h"
#include "rendering/chunkRenderer.h"
#include "rendering/Crosshair.h"
#include "util/paths.h"
#include "util/utils.h"
#include "util/timer.h"

void mouse_button_callback(GLFWwindow *window, int button, int action, int mods);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow* window, double xPos, double yPos);
void scroll_callback(GLFWwindow* window, double xOffset, double yOffset);
void find_spawn(World& world, Camera& camera);
void APIENTRY glDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity,
    GLsizei length, const GLchar* message, const void* userParam);

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
float speed = 32.0f;
int renderDistance = 32;

// world
bool firstRun = true;
bool breakRequested = false;
bool placeRequested = false;
float dayLengthSeconds = 60.0f * 10.0f;

int main() {
    {
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifndef NDEBUG
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif

        GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);
        int scrWidth = mode->width;
        int scrHeight = mode->height;
        GLFWwindow* window = glfwCreateWindow(scrWidth, scrHeight, "LearnOpenGL", nullptr, nullptr);
        if (window == nullptr) {
            std::cout << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
            return -1;
        }
        glfwMakeContextCurrent(window);
        glfwSwapInterval(0); // 1 = vsync, 0 = uncapped
        glfwSetMouseButtonCallback(window, mouse_button_callback);
        glfwSetCursorPosCallback(window, mouse_callback);
        glfwSetScrollCallback(window, scroll_callback);
        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

        if (!gladLoadGL(glfwGetProcAddress)) {
            std::cout << "Failed to initialize GLAD" << std::endl;
            return -1;
        }

#ifndef NDEBUG
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(glDebugCallback, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION,
            0, nullptr, GL_FALSE);
#endif

        Framebuffer sceneFbo;
        VertexArray fbVao; // empty array for framebuffer
        World world;
        ChunkRenderer chunkRenderer(world);
        ChunkLoader chunkLoader;
        Texture2DArray blockTextures;
        VertexArray blankVertexArray; // blank array for skybox
        blockTextures.load({
            resourcePath("textures/dirt.png"),
            resourcePath("textures/grass_block_side.png"),
            resourcePath("textures/grass_block_top.png"),
            resourcePath("textures/stone.png")
        }, true);
        Shader shader(
                resourcePath("shaders/chunk.vert").c_str(),
                resourcePath("shaders/chunk.frag").c_str()
        );
        Shader skyboxShader(
            resourcePath("shaders/skybox.vert").c_str(),
            resourcePath("shaders/skybox.frag").c_str()
        );
        Shader fxaaShader(
            resourcePath("shaders/fxaa.vert").c_str(),
            resourcePath("shaders/fxaa.frag").c_str()
            );
        chunkRenderer.init();

        Crosshair crosshair(
            resourcePath("shaders/crosshair.vert").c_str(),
            resourcePath("shaders/crosshair.frag").c_str()
        );
        BlockOutline blockOutline(
            resourcePath("shaders/outline.vert").c_str(),
            resourcePath("shaders/outline.frag").c_str()
        );

        // time
        float timeOfDay = 0.0f;
        glm::vec3 sunDir;

        // fps
        double lastFPSTime = glfwGetTime();

        // enabling depth testing and back-face culling
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);

        // locking mouse
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        // GAME ------------------------
        find_spawn(world, camera);

        chunkLoader.start(world);

        glm::ivec3 lastPlayerChunk = worldToChunkPos(camera.position);

        const float horizonOffset = -0.15f;
        const float gradientPower = 0.6f;
        const glm::vec3 nightZenith(0.01f, 0.015f, 0.05f), nightHorizon(0.03f, 0.04f, 0.09f);
        const glm::vec3 duskZenith (0.18f, 0.25f, 0.50f),  duskHorizon (0.95f, 0.45f, 0.20f);
        const glm::vec3 dayZenith  (72.f/256.f, 195.f/256.f, 231.f/256.f);
        const glm::vec3 dayHorizon (64.f/256.f, 160.f/256.f, 200.f/256.f);

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

            // time
            timeOfDay += deltaTime / dayLengthSeconds;
            float a = timeOfDay * 2.f * glm::pi<float>();
            sunDir = glm::normalize(glm::vec3(std::cos(a), std::sin(a), 0.2f));
            // sky color computing
            float h = sunDir.y;
            float twilightT = glm::smoothstep(-0.25f, 0.0f, h);   // 0 = night, 1 = sun at the horizon
            float dayT = glm::smoothstep( 0.0f, 0.35f, h);   // 0 = sun at the horizon, 1 = full day

            glm::vec3 zenithColor = mix(glm::mix(nightZenith,  duskZenith,  twilightT), dayZenith,  dayT);
            glm::vec3 horizonColor = mix(glm::mix(nightHorizon, duskHorizon, twilightT), dayHorizon, dayT);
            glm::vec3 sunColor = mix(glm::vec3(1.0f, 0.5f, 0.2f), glm::vec3(1.0f, 0.96f, 0.85f), dayT);

            // the sky's color exactly on the horizon line (same formula as the shader at dir.y = 0)
            float t0 = std::pow(-horizonOffset / (1.0f - horizonOffset), gradientPower);
            glm::vec3 fogColor = glm::mix(horizonColor, zenithColor, t0);

            glm::ivec3 currentPlayerChunk = worldToChunkPos(camera.position);
            if (currentPlayerChunk != lastPlayerChunk || firstRun) {
                chunkLoader.setPlayerChunk(currentPlayerChunk);

                // unloading distant chunks
                auto chunksToUnload = world.getChunksToUnload(currentPlayerChunk, renderDistance);
                chunkLoader.requestUnload(chunksToUnload);

                // requesting chunks
                int half = renderDistance / 2;
                int vertHalf = half / 2;
                for (int x = -half; x < half; x++)
                    for (int y = -vertHalf; y < vertHalf; y++)
                        for (int z = -half; z < half; z++) {
                            glm::ivec3 coord = currentPlayerChunk + glm::ivec3(x, y, z);
                            if (!world.hasChunk(coord)) chunkLoader.requestChunk(coord);
                        }

                lastPlayerChunk = currentPlayerChunk;
                firstRun = false;
            }

            // looking at block
            float reach = 6.0f;
            std::optional<glm::ivec3> target;
            glm::vec3 hitNormal = glm::vec3(0);
            Raycast ray(camera.position, camera.position + camera.target * reach);
            while (auto p = ray.getNextTargetPos()) {
                if (world.getBlock(*p) != 0) {
                    hitNormal = ray.getNormal();
                    target = *p;
                    break;
                }
            }

            // breaking blocks
            if (breakRequested) {
                breakRequested = false;
                if (target) {
                    world.setBlock(*target, 0);
                }
            }

            // placing blocks
            if (placeRequested) {
                placeRequested = false;
                if (target) {
                    glm::ivec3 placeBlock = *target + (glm::ivec3)normalize(hitNormal);
                    world.setBlock( + placeBlock, 1);
                }
            }

            // input
            // -----
            processInput(window);

            // render
            // ------
            int fbW, fbH;
            glfwGetFramebufferSize(window, &fbW, &fbH);
            sceneFbo.resize(fbW, fbH);
            sceneFbo.bind();
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // view and projection
            glm::mat4 view, proj;
            view = camera.getViewMatrix();
            proj = camera.getProjectionMatrix();
            camera.updateFrustum();

            // setting uniforms
            shader.use();
            blockTextures.bind(0);
            shader.setInt("u_texture", 0);
            shader.setMat4("u_view", view);
            shader.setMat4("u_proj", proj);
            shader.setVec3("u_ambient", glm::vec3(0.3f));
            shader.setVec3("u_sunDir", sunDir);
            shader.setVec3("u_fogColor", fogColor);
            shader.setVec3("u_zenithColor", zenithColor);
            shader.setVec3("u_sunColor", sunColor);
            shader.setVec3("u_camPos", camera.position);
            shader.setVec2("u_fog", glm::vec2((renderDistance / 2.0f * 0.75f) * 16.0f, renderDistance * 8.0f));

            chunkLoader.queueDirty(world);
            chunkLoader.processUnload(world, chunkRenderer);
            chunkLoader.processReady(world, chunkRenderer);

            // chunk render
            chunkRenderer.drawAll(shader, camera);

            // outline render
            if (target) blockOutline.draw(*target, view, proj);

            // skybox render
            glDepthFunc(GL_LEQUAL);
            glDisable(GL_CULL_FACE);
            skyboxShader.use();
            blankVertexArray.bind();
            skyboxShader.setMat4("u_invViewProj", glm::inverse(proj * glm::mat4(glm::mat3(view)))); // mat3 to delete translation
            skyboxShader.setVec3("u_horizonColor", horizonColor);
            skyboxShader.setVec3("u_zenithColor", zenithColor);
            skyboxShader.setVec3("u_sunColor", sunColor);
            skyboxShader.setVec3("u_sunDir", sunDir);
            skyboxShader.setFloat("u_horizonOffset", horizonOffset);
            skyboxShader.setFloat("u_gradientPower", gradientPower);
            glDrawArrays(GL_TRIANGLES, 0, 3);
            glDepthFunc(GL_LESS);
            glEnable(GL_CULL_FACE);

            // framebuffer stuff
            Framebuffer::bindDefault(fbW, fbH);
            glDisable(GL_CULL_FACE);
            glDisable(GL_DEPTH_TEST);

            // fxaa
            fxaaShader.use();
            sceneFbo.bindColorTexture(0);
            fxaaShader.setInt("u_scene", 0);
            fxaaShader.setVec2("u_texelSize", glm::vec2(1.0f / fbW, 1.0f / fbH));
            fbVao.bind();
            glDrawArrays(GL_TRIANGLES, 0, 3);

            glEnable(GL_DEPTH_TEST);
            glEnable(GL_CULL_FACE);
            // frambuffer end

            // crosshair
            framebuffer_size_callback(window, fbW, fbH);
            crosshair.draw(fbW, fbH);

            // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
            // -------------------------------------------------------------------------------
            glfwSwapBuffers(window);
            glfwPollEvents();
        }

        chunkLoader.stop();
    }

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

void mouse_button_callback(GLFWwindow *window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) breakRequested = true;
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) placeRequested = true;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    if (width == 0 || height == 0) return;
    glViewport(0, 0, width, height);
    camera.aspect = (float)width / (float)height;
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

// prints gl debug, only in debug builds
void glDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam) {
    std::cerr << "[GL] " << message << std::endl;
}


