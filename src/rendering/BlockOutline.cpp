#include "BlockOutline.h"
#include "BlockOutline.h"
#include "BlockOutline.h"
#include "BlockOutline.h"
#include "BlockOutline.h"
#include "BlockOutline.h"
#include "BlockOutline.h"

#include "../constants.h"

void BlockOutline::draw(const glm::ivec3 blockPos, const glm::mat4 &view, const glm::mat4 &proj, int fbWidth, int fbHeight) {
    shader.use();
    shader.setMat4("u_view", view);
    shader.setMat4("u_proj", proj);
    shader.setVec3("u_worldPos", glm::vec3(blockPos));
    shader.setVec2("u_screenSize", glm::vec2(fbWidth, fbHeight));
    shader.setFloat("u_thickness", thickness);
    mesh.draw();
}
