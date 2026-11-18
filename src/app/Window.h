#ifndef WINDOW_H
#define WINDOW_H

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <stdexcept>

class Window {
public:
    Window(const Window &) = delete;
    Window &operator=(const Window &) = delete;
    Window(int width, int height, const char *title) {
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifndef NDEBUG
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif
        m_window = glfwCreateWindow(width, height, title, nullptr, nullptr);
        if (!m_window) {
            glfwTerminate();
            throw std::runtime_error("Failed to create GLFW window");
        }
        glfwMakeContextCurrent(m_window);

        if (!gladLoadGL(glfwGetProcAddress)) {
            glfwDestroyWindow(m_window);
            glfwTerminate();
            throw std::runtime_error("Failed to initialize GLAD");
        }

        glfwSwapInterval(0); // 1 = vsync
        glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        m_windowedSize = {width, height};
    }

    ~Window() {
        glfwDestroyWindow(m_window);
        glfwTerminate();
    }

    GLFWwindow *handle() const { return m_window; }
    bool shouldClose() const { return glfwWindowShouldClose(m_window); }
    void close() { glfwSetWindowShouldClose(m_window, true); }

    void swapAndPoll() {
        glfwSwapBuffers(m_window);
        glfwPollEvents();
    }

    bool key(int k) const { return glfwGetKey(m_window, k) == GLFW_PRESS; }

    glm::ivec2 framebufferSize() const {
        glm::ivec2 s;
        glfwGetFramebufferSize(m_window, &s.x, &s.y);
        return s;
    }

    void toggleFullscreen() {
        m_fullscreen = !m_fullscreen;
        if (m_fullscreen) {
            glfwGetWindowPos(m_window, &m_windowedPos.x, &m_windowedPos.y);
            glfwGetWindowSize(m_window, &m_windowedSize.x, &m_windowedSize.y);
            GLFWmonitor *mon = glfwGetPrimaryMonitor();
            const GLFWvidmode *mode = glfwGetVideoMode(mon);
            glfwSetWindowMonitor(m_window, mon, 0, 0, mode->width, mode->height, mode->refreshRate);
        } else {
            glfwSetWindowMonitor(m_window, nullptr, m_windowedPos.x, m_windowedPos.y,
                                 m_windowedSize.x, m_windowedSize.y, 0);
        }
    }

private:
    GLFWwindow *m_window = nullptr;
    bool m_fullscreen = false;
    glm::ivec2 m_windowedPos{100, 100}, m_windowedSize{1280, 720};
};

#endif //WINDOW_H
