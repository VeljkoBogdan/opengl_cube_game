#ifndef SKYBOX_H
#define SKYBOX_H

#include <glad/glad.h>

#include "Sky.h"
#include "../util/paths.h"
#include "../shader.h"
#include "../gl/VertexArray.h"

class Skybox {
public:
    Skybox()
        : m_shader(resourcePath("shaders/skybox.vert").c_str(),
                   resourcePath("shaders/skybox.frag").c_str()) {}

    void draw(const SkyState& sky, const glm::mat4& view, const glm::mat4& proj,
              const SkyParams& params = {}) {
        glDepthFunc(GL_LEQUAL);
        glDisable(GL_CULL_FACE);

        m_shader.use();
        m_vao.bind();
        m_shader.setMat4("u_invViewProj", glm::inverse(proj * glm::mat4(glm::mat3(view)))); // strip translation
        m_shader.setVec3("u_horizonColor", sky.horizon);
        m_shader.setVec3("u_zenithColor",  sky.zenith);
        m_shader.setVec3("u_sunColor",     sky.sunColor);
        m_shader.setVec3("u_sunDir",       sky.sunDir);
        m_shader.setFloat("u_horizonOffset", params.horizonOffset);
        m_shader.setFloat("u_gradientPower", params.gradientPower);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glDepthFunc(GL_LESS);
        glEnable(GL_CULL_FACE);
    }

private:
    Shader m_shader;
    VertexArray m_vao;
};

#endif //SKYBOX_H
