#include "UIRenderer.h"

#include <algorithm>
#include <cstddef>
#include <glm/gtc/matrix_transform.hpp>

void UIRenderer::init(const std::string& vertPath, const std::string& fragPath) {
    shader = std::make_unique<Shader>(vertPath.c_str(), fragPath.c_str());

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    vbo.bind();

    const GLsizei stride = sizeof(Vertex);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(Vertex, x));
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(Vertex, u));
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(Vertex, r));
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(Vertex, layer));
    for (int i = 0; i < 4; ++i) glEnableVertexAttribArray(i);
    glBindVertexArray(0);
}

void UIRenderer::begin(int fbW, int fbH, int baseHeight) {
    scale_ = std::max(1, fbH / baseHeight); // integer scale keeps pixel art crisp
    virtW_ = float(fbW) / scale_;
    virtH_ = float(fbH) / scale_;
    verts.clear();
}

void UIRenderer::quad(float x, float y, float w, float h, glm::vec4 uv, glm::vec4 c, float layer) {
    const float x1 = x + w, y1 = y + h;
    const float u0 = uv.x, v0 = uv.y, u1 = uv.z, v1 = uv.w;
    verts.push_back({x,  y,  u0, v0, c.r, c.g, c.b, c.a, layer});
    verts.push_back({x1, y,  u1, v0, c.r, c.g, c.b, c.a, layer});
    verts.push_back({x1, y1, u1, v1, c.r, c.g, c.b, c.a, layer});
    verts.push_back({x,  y,  u0, v0, c.r, c.g, c.b, c.a, layer});
    verts.push_back({x1, y1, u1, v1, c.r, c.g, c.b, c.a, layer});
    verts.push_back({x,  y1, u0, v1, c.r, c.g, c.b, c.a, layer});
}

void UIRenderer::rect(float x, float y, float w, float h, glm::vec4 color) {
    quad(x, y, w, h, {0, 0, 1, 1}, color, -1.0f);
}

void UIRenderer::image(float x, float y, float w, float h, int layer, glm::vec4 tint, glm::vec4 uvRect) {
    quad(x, y, w, h, uvRect, tint, float(layer));
}

void UIRenderer::end(GLuint textureArray) {
    if (verts.empty()) return;

    const GLboolean depthWas = glIsEnabled(GL_DEPTH_TEST);
    const GLboolean cullWas  = glIsEnabled(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    shader->use();
    shader->setMat4("u_proj", glm::ortho(0.0f, virtW_, virtH_, 0.0f));
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_ARRAY, textureArray);
    shader->setInt("u_tex", 0);

    glBindVertexArray(vao);
    vbo.setData(verts, GL_STREAM_DRAW);
    glDrawArrays(GL_TRIANGLES, 0, GLsizei(verts.size()));
    glBindVertexArray(0);

    glDisable(GL_BLEND);
    if (depthWas) glEnable(GL_DEPTH_TEST);
    if (cullWas)  glEnable(GL_CULL_FACE);
}

glm::vec2 UIRenderer::place(Anchor a, glm::vec2 off, glm::vec2 size) const {
    float ax = 0, ay = 0;
    switch (a) {
        case Anchor::TopLeft:     ax = 0;                     ay = 0; break;
        case Anchor::Top:         ax = (virtW_ - size.x) / 2; ay = 0; break;
        case Anchor::TopRight:    ax = virtW_ - size.x;       ay = 0; break;
        case Anchor::Left:        ax = 0;                     ay = (virtH_ - size.y) / 2; break;
        case Anchor::Center:      ax = (virtW_ - size.x) / 2; ay = (virtH_ - size.y) / 2; break;
        case Anchor::Right:       ax = virtW_ - size.x;       ay = (virtH_ - size.y) / 2; break;
        case Anchor::BottomLeft:  ax = 0;                     ay = virtH_ - size.y; break;
        case Anchor::Bottom:      ax = (virtW_ - size.x) / 2; ay = virtH_ - size.y; break;
        case Anchor::BottomRight: ax = virtW_ - size.x;       ay = virtH_ - size.y; break;
    }
    return {ax + off.x, ay + off.y};
}