#ifndef UIRENDERER_H
#define UIRENDERER_H

#include <memory>
#include <string>
#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>

#include "../../shader.h"
#include "../../gl/GLBuffer.h"

enum class Anchor { TopLeft, Top, TopRight, Left, Center, Right, BottomLeft, Bottom, BottomRight };

class UIRenderer {
public:
    UIRenderer() = default;
    ~UIRenderer() { if (vao) glDeleteVertexArrays(1, &vao); }
    UIRenderer(const UIRenderer&) = delete;
    UIRenderer& operator=(const UIRenderer&) = delete;

    void init(const std::string& vertPath, const std::string& fragPath);

    void begin(int fbW, int fbH, int baseHeight = 360);
    void rect(float x, float y, float w, float h, glm::vec4 color);
    void image(float x, float y, float w, float h, int layer,
               glm::vec4 tint = {1, 1, 1, 1}, glm::vec4 uvRect = {0, 0, 1, 1});
    void end(GLuint textureArray = 0);

    [[nodiscard]] int   scale() const { return scale_; }
    [[nodiscard]] float virtW() const { return virtW_; }
    [[nodiscard]] float virtH() const { return virtH_; }
    [[nodiscard]] glm::vec2 place(Anchor a, glm::vec2 offset, glm::vec2 size) const;
    [[nodiscard]] glm::vec2 toVirtual(glm::vec2 fbPixels) const { return fbPixels / float(scale_); }

private:
    struct Vertex { float x, y, u, v, r, g, b, a, layer; };
    void quad(float x, float y, float w, float h, glm::vec4 uv, glm::vec4 c, float layer);

    std::unique_ptr<Shader> shader;
    GLBuffer vbo{GL_ARRAY_BUFFER};
    GLuint vao = 0;
    std::vector<Vertex> verts;
    int scale_ = 1;
    float virtW_ = 0, virtH_ = 0;
};

#endif