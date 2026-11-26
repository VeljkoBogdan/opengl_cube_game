#include "Game.h"
#include "../gl/DebugCallback.h"
#include "../util/utils.h"

Game::Game()
    : chunkShader("shaders/chunk.vert", "shaders/chunk.frag"),
      crosshair("shaders/crosshair.vert", "shaders/crosshair.frag"),
      blockOutline("shaders/outline.vert", "shaders/outline.frag", 2.0f)
{
    installGLDebugCallback();

    GLFWwindow* w = window.handle();
    glfwSetWindowUserPointer(w, this);
    glfwSetKeyCallback(w, [](GLFWwindow* w, int k, int, int a, int) { self(w)->onKey(k, a); });
    glfwSetMouseButtonCallback(w, [](GLFWwindow* w, int b, int a, int) { self(w)->onMouseButton(b, a); });
    glfwSetCursorPosCallback(w, [](GLFWwindow* w, double x, double y) { self(w)->player.mouseCallback(x, y); });
    glfwSetFramebufferSizeCallback(w, [](GLFWwindow* w, int fw, int fh) { self(w)->onResize(fw, fh); });

    blockTextures.load({ "textures/dirt.png",
        "textures/grass_block_side.png",
        "textures/grass_block_top.png",
        "textures/stone.png",
        "textures/oak_log.png",
        "textures/oak_log_top.png",
        "textures/oak_leaves.png"
    }, true);
    chunkRenderer.init();
    ui.init("shaders/ui.vert", "shaders/ui.frag");

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    player.position = glm::vec3(0.0f, 80.0f, 0.0f);
    const auto fb = window.framebufferSize();
    onResize(fb.x, fb.y);
    chunkLoader.start(world);
}

Game::~Game() { chunkLoader.stop(); }

void Game::run() {
    while (!window.shouldClose()) {
        updateTime();
        updateChunkStreaming();

        auto target = raycastBlock(world, player.getCamera(), kReach);
        if (target && breakRequested) breakBlock(world, *target);
        if (target && placeRequested) placeBlock(world, *target, 1);
        breakRequested = placeRequested = false;

        player.move(world, readMoveInput(), deltaTime);
        player.update(deltaTime);

        const SkyState sky = computeSky(timeOfDay);
        const glm::ivec2 fb = window.framebufferSize();
        renderScene(sky, target, fb);
        post.resolve(sceneFbo, fb.x, fb.y);
        renderUI(fb);

        window.swapAndPoll();
    }
}

void Game::updateTime() {
    const float now = (float)glfwGetTime();
    deltaTime = now - lastTime;
    lastTime = now;
    timeOfDay += deltaTime / kDayLengthSeconds;
}

void Game::updateChunkStreaming() {
    const glm::ivec3 cur = worldToChunkPos(player.getCamera().position);
    if (cur == lastPlayerChunk) return;
    chunkLoader.updateAround(world, cur, player.getRenderDistance());
    lastPlayerChunk = cur;
}

glm::vec3 Game::readMoveInput() const {
    auto axis = [&](int pos, int neg) { return float(window.key(pos)) - float(window.key(neg)); };
    return { axis(GLFW_KEY_D, GLFW_KEY_A),
             axis(GLFW_KEY_SPACE, GLFW_KEY_LEFT_SHIFT),
             axis(GLFW_KEY_W, GLFW_KEY_S) };
}

void Game::onKey(int key, int action) {
    if (action != GLFW_PRESS) return;
    switch (key) {
        case GLFW_KEY_ESCAPE: window.close(); break;
        case GLFW_KEY_P:      post.fxaaEnabled = !post.fxaaEnabled; break;
        case GLFW_KEY_F11:    window.toggleFullscreen(); break;
    }
}

void Game::onMouseButton(int button, int action) {
    if (action != GLFW_PRESS) return;
    if (button == GLFW_MOUSE_BUTTON_LEFT)  breakRequested = true;
    if (button == GLFW_MOUSE_BUTTON_RIGHT) placeRequested = true;
}

void Game::onResize(int w, int h) {
    if (w == 0 || h == 0) return;
    glViewport(0, 0, w, h);
    player.getCamera().aspect = (float)w / h;
}

void Game::renderScene(const SkyState& sky, const std::optional<BlockHit>& target, glm::ivec2 fb) {
    sceneFbo.resize(fb.x, fb.y);
    sceneFbo.bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    const glm::mat4 view = player.getCamera().getViewMatrix();
    const glm::mat4 proj = player.getCamera().getProjectionMatrix();

    setChunkUniforms(sky, view, proj);
    chunkLoader.queueDirty(world);
    chunkLoader.processUnload(world, chunkRenderer);
    chunkLoader.processReady(world, chunkRenderer);
    chunkRenderer.drawAll(chunkShader, player.getCamera());

    if (target) blockOutline.draw(target->pos, view, proj, fb.x, fb.y);
    skybox.draw(sky, view, proj);
}

void Game::setChunkUniforms(const SkyState& sky, const glm::mat4& view, const glm::mat4& proj) {
    const float rd = (float)player.getRenderDistance();
    chunkShader.use();
    blockTextures.bind(0);
    chunkShader.setInt("u_texture", 0);
    chunkShader.setMat4("u_view", view);
    chunkShader.setMat4("u_proj", proj);
    chunkShader.setVec3("u_ambient", glm::vec3(0.3f));
    chunkShader.setVec3("u_sunDir", sky.sunDir);
    chunkShader.setVec3("u_fogColor", sky.fog);
    chunkShader.setVec3("u_zenithColor", sky.zenith);
    chunkShader.setVec3("u_sunColor", sky.sunColor);
    chunkShader.setVec3("u_camPos", player.getCamera().position);
    chunkShader.setVec2("u_fog", glm::vec2(rd / 2.0f * 0.75f * 16.0f, rd * 8.0f));
}

void Game::renderUI(glm::ivec2 fb) {
    ui.begin(fb.x, fb.y);
    const glm::vec2 size{182, 22};
    const glm::vec2 pos = ui.place(Anchor::Bottom, {0, -4}, size);
    ui.rect(pos.x, pos.y, size.x, size.y, {0, 0, 0, 0.5f});
    ui.end();

    crosshair.draw(fb.x, fb.y);
}