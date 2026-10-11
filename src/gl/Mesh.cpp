#include "Mesh.h"

void Mesh::setVertices(const std::vector<float> &data, const VertexLayout &layout, GLenum usage) {
    vbo.setData(data, usage);
    if (!configured) {
        vao.addBuffer(vbo, layout);
        configured = true;
    }
    vertexCount = (GLsizei)(data.size() * sizeof(float) / layout.getStride());
}

void Mesh::draw(GLenum mode) {
    if (vertexCount <= 0) return;
    vao.bind();
    glDrawArrays(mode, 0, vertexCount);
}

GLsizei Mesh::getVertexCount() const {
    return vertexCount;
}
