#include "GLBuffer.h"

#include "GLFW/glfw3.h"

GLBuffer& GLBuffer::operator=(GLBuffer &&obj) noexcept {
    if (this != &obj) {
        destroy();
        id = obj.id;
        target = obj.target;
        obj.id = 0;
    }
    return *this;
}

void GLBuffer::allocate(size_t bytes, GLenum usage) {
    setData(nullptr, bytes, usage);
}

void GLBuffer::setData(const void *data, size_t bytes, GLenum usage) {
    bind();
    glBufferData(target, (GLsizeiptr)bytes, data, usage);
}

void GLBuffer::update(size_t offsetBytes, const void *data, size_t bytes) {
    bind();
    glBufferSubData(target, (GLintptr)offsetBytes, (GLsizeiptr)bytes, data);
}

void GLBuffer::bind() {
    if (!id) glGenBuffers(1, &id);
    glBindBuffer(target, id);
}

GLuint GLBuffer::getId() const {
    return id;
}

void GLBuffer::destroy() {
    if (id && glfwGetCurrentContext()) glDeleteBuffers(1, &id);
    id = 0;
}
