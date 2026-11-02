#include "BlockOutline.h"

#include "../constants.h"

void BlockOutline::draw(const glm::ivec3 blockPos, const glm::mat4 &view, const glm::mat4 &proj) {
    shader.use();
    shader.setMat4("u_view", view);
    shader.setMat4("u_proj", proj);
    shader.setVec3("u_worldPos", glm::vec3(blockPos));
    mesh.draw(GL_LINES);
}
