#include "VertexArray.h"

#include "GLFW/glfw3.h"

VertexLayout& VertexLayout::floats(GLuint count) {
    return push(count, GL_FLOAT, false, false, sizeof(float));
}

VertexLayout& VertexLayout::bytes(GLuint count, bool normalized) {
    return push(count, GL_UNSIGNED_BYTE, normalized, false, 1);
}

VertexLayout& VertexLayout::uints(GLuint count) {
    return push(count, GL_UNSIGNED_INT, false, true, sizeof(GLuint));
}

VertexLayout& VertexLayout::perInstance(GLuint divisor) {
    this->divisor = divisor;
    return *this;
}

const std::vector<VertexLayout::Element> & VertexLayout::getElements() const {
    return elements;
}

GLsizei VertexLayout::getStride() const {
    return (GLsizei)stride;
}

GLuint VertexLayout::getDivisor() const {
    return divisor;
}

VertexLayout& VertexLayout::push(GLuint count, GLenum type, bool normalized, bool integer, size_t size) {
    elements.push_back({count, type, normalized, integer, stride});
    stride += count * size;
    return *this;
}

VertexArray& VertexArray::operator=(VertexArray &&obj) noexcept {
    if (this != &obj) {
        destroy();
        id = obj.id;
        nextLocation = obj.nextLocation;
        obj.id = 0;
    }
    return *this;
}

void VertexArray::addBuffer(GLBuffer &buffer, const VertexLayout &layout) {
    bind();
    buffer.bind();
    for (const auto& element : layout.getElements()) {
        const void* offset = reinterpret_cast<const void*>(element.offset);
        if (element.integer) glVertexAttribIPointer(nextLocation, element.count, element.type, layout.getStride(), offset);
        else glVertexAttribPointer(nextLocation, element.count, element.type, element.normalized ? GL_TRUE : GL_FALSE, layout.getStride(), offset);
        glEnableVertexAttribArray(nextLocation);
        glVertexAttribDivisor(nextLocation, layout.getDivisor());
        nextLocation++;
    }
}

void VertexArray::bind() {
    if (!id) glGenVertexArrays(1, &id);
    glBindVertexArray(id);
}

void VertexArray::destroy() {
    if (id && glfwGetCurrentContext()) glDeleteVertexArrays(1, &id);
    id = 0;
    nextLocation = 0;
}


