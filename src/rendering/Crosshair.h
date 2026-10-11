#ifndef OPENGL_TEST_CROSSHAIR_H
#define OPENGL_TEST_CROSSHAIR_H
#include <vector>

#include "../shader.h"
#include "../gl/Mesh.h"
#include "../gl/VertexArray.h"

class Crosshair {
public:
    Crosshair(const char* vertPath, const char* fragPath, float armLength = 10.0f, float thickness = 2.0f)
        : shader(vertPath, fragPath) {
        const float h = thickness * 0.5f;
        const float l = armLength;

        std::vector<float> v;
        auto rect = [&](float x0, float y0, float x1, float y1) {
            v.insert(v.end(), {
                x0, y0, x1, y0, x1, y1,
                x1, y1, x0, y1, x0, y0
            });
        };

        rect(-l, -h, l, h);
        rect(-h, h, h, l);
        rect(-h, -l, h, -h);

        mesh.setVertices(v, VertexLayout().floats(2));
    }

    void draw(int framebufferWidth, int framebufferHeight) {
        GLboolean depthWasOn = glIsEnabled(GL_DEPTH_TEST);
        GLboolean cullWasOn = glIsEnabled(GL_CULL_FACE);

        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        glEnable(GL_BLEND);

        glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ONE_MINUS_SRC_COLOR);

        shader.use();
        shader.setFloat("u_width", (float)framebufferWidth);
        shader.setFloat("u_height", (float)framebufferHeight);
        mesh.draw();

        glDisable(GL_BLEND);
        if (depthWasOn) glEnable(GL_DEPTH_TEST);
        if (cullWasOn) glEnable(GL_CULL_FACE);
    }
private:
    Shader shader;
    Mesh mesh;
};

#endif //OPENGL_TEST_CROSSHAIR_H
