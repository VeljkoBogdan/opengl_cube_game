#ifndef OPENGL_TEST_MESH_H
#define OPENGL_TEST_MESH_H
#include <vector>

#include "VertexArray.h"

class Mesh {
public:
    void setVertices(const std::vector<float>& data, const VertexLayout& layout, GLenum usage = GL_DYNAMIC_DRAW);
    void draw(GLenum mode = GL_TRIANGLES);

    [[nodiscard]] GLsizei getVertexCount() const;
private:
    GLBuffer vbo{GL_ARRAY_BUFFER};
    VertexArray vao;
    GLsizei vertexCount = 0;
    bool configured = false;
};

#endif //OPENGL_TEST_MESH_H
