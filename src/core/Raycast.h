#ifndef OPENGL_TEST_RAYCAST_H
#define OPENGL_TEST_RAYCAST_H
#include <optional>

#include "glm/vec3.hpp"

class Raycast {
public:
    enum Context {
        Block
    };

    Raycast(glm::vec3 from, glm::vec3 to, Context raycastContext = Block);
    std::optional<glm::ivec3> getNextTargetPos();
    glm::vec3 getNormal();
private:
    Context raycastContext;
    glm::vec3 normal;
    glm::vec3 from, to;
    glm::ivec3 pos;
    int stepX, stepY, stepZ;
    double deltaX, deltaY, deltaZ;
    double tmaxX, tmaxY, tmaxZ;
    int steps = 0;
};

#endif //OPENGL_TEST_RAYCAST_H
