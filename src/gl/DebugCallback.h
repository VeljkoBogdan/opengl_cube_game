#ifndef DEBUGCALLBACK_H
#define DEBUGCALLBACK_H

#include <glad/glad.h>
#include <iostream>

#ifndef NDEBUG
namespace detail {
    inline void APIENTRY glDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity,
                                         GLsizei length, const GLchar* message, const void* userParam) {
        std::cerr << "[GL] " << message << std::endl;
    }
}
#endif

inline void installGLDebugCallback() {
#ifndef NDEBUG
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(detail::glDebugCallback, nullptr);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION,
                          0, nullptr, GL_FALSE);
#endif
}

#endif // DEBUGCALLBACK_H