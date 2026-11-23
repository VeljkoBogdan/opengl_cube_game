#ifndef POSTPROCESS_H
#define POSTPROCESS_H

#include "../util/paths.h"
#include "../gl/Framebuffer.h"
#include "../gl/VertexArray.h"
#include "../shader.h"

class PostProcess {
public:
    PostProcess()
        : m_fxaa("shaders/fxaa.vert",
                 "shaders/fxaa.frag") {}

    bool fxaaEnabled = true;

    // Call between scene rendering and UI. Draws sceneFbo to the default framebuffer.
    void resolve(Framebuffer& scene, int w, int h) {
        Framebuffer::bindDefault(w, h);
        glDisable(GL_CULL_FACE);
        glDisable(GL_DEPTH_TEST);

        m_fxaa.use();
        scene.bindColorTexture(0);
        m_fxaa.setInt("u_scene", 0);
        m_fxaa.setVec2("u_texelSize", glm::vec2(1.0f / w, 1.0f / h));
        m_fxaa.setBool("u_enabled", fxaaEnabled);
        m_vao.bind();
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
    }

private:
    Shader m_fxaa;
    VertexArray m_vao;
};

#endif //POSTPROCESS_H
