#ifndef OPENGL_TEST_VERTEXARRAY_H
#define OPENGL_TEST_VERTEXARRAY_H
#include <vector>

#include "GLBuffer.h"
#include "glad/glad.h"

class VertexLayout {
public:
    struct Element {
        GLuint count;
        GLenum type;
        bool normalized;
        bool integer;
        size_t offset;
    };

    VertexLayout& floats(GLuint count);
    VertexLayout& bytes(GLuint count, bool normalized = true);
    VertexLayout& uints(GLuint count);
    VertexLayout& perInstance(GLuint divisor = 1);

    [[nodiscard]] const std::vector<Element>& getElements() const;
    [[nodiscard]] GLsizei getStride() const;
    [[nodiscard]] GLuint getDivisor() const;

private:
    VertexLayout& push(GLuint count, GLenum type, bool normalized, bool integer, size_t size);

    std::vector<Element> elements;
    size_t stride = 0;
    GLuint divisor = 0;
};

class VertexArray {
public:
    VertexArray() = default;
    ~VertexArray() { destroy(); }

    VertexArray(const VertexArray&) = delete;
    VertexArray& operator=(const VertexArray&) = delete;
    VertexArray(VertexArray&& obj) noexcept : id(obj.id), nextLocation(obj.nextLocation) { obj.id = 0; }
    VertexArray& operator=(VertexArray&& obj) noexcept;
    void addBuffer(GLBuffer& buffer, const VertexLayout& layout);
    void bind();
    void destroy();
private:
    GLuint id = 0;
    GLuint nextLocation = 0;
};

#endif //OPENGL_TEST_VERTEXARRAY_H
