#ifndef OPENGL_TEST_GLBUFFER_H
#define OPENGL_TEST_GLBUFFER_H
#include <vector>

#include "glad/glad.h"

class GLBuffer {
public:
    explicit GLBuffer(GLenum target = GL_ARRAY_BUFFER) : target(target) {}
    ~GLBuffer() { destroy(); }

    GLBuffer(const GLBuffer&) = delete;
    GLBuffer& operator=(const GLBuffer&) = delete;
    GLBuffer(GLBuffer&& obj) noexcept : id(obj.id), target(obj.target) { obj.id = 0; }
    GLBuffer& operator=(GLBuffer&& obj) noexcept;

    void allocate(size_t bytes, GLenum usage = GL_DYNAMIC_DRAW);
    void setData(const void* data, size_t bytes, GLenum usage = GL_DYNAMIC_DRAW);
    template <class T>
    void setData(const std::vector<T>& vector, GLenum usage = GL_DYNAMIC_DRAW);
    void update(size_t offsetBytes, const void* data, size_t bytes);
    void bind();
    [[nodiscard]] GLuint getId() const;

    void destroy();
private:
    GLuint id = 0;
    GLenum target;
};

template<class T>
void GLBuffer::setData(const std::vector<T> &vector, GLenum usage) {
    bind();
    setData(vector.data(), vector.size() * sizeof(T), usage);
}

#endif //OPENGL_TEST_GLBUFFER_H
