#ifndef OPENGL_TEST_BLOCKOUTLINE_H
#define OPENGL_TEST_BLOCKOUTLINE_H
#include "../shader.h"
#include "../gl/Mesh.h"

class BlockOutline {
public:
    BlockOutline(const char* vertPath, const char* fragPath, float thickness = 1.0f)
    : shader(vertPath, fragPath), thickness(thickness) {
        std::vector<float> v;
        for (int axis = 0; axis < 3; axis++) {
            int u = (axis + 1) % 3;
            int w = (axis + 2) % 3;
            constexpr float a = 0.502f;
            for (int su = -1; su <= 1; su += 2)
            for (int sw = -1; sw <= 1; sw += 2) {
                glm::vec3 p0, p1;
                p0[axis] = -a;
                p1[axis] = a;
                p0[u] = p1[u] = su * a;
                p0[w] = p1[w] = sw * a;
                v.insert(v.end(), { p0.x, p0.y, p0.z, p1.x, p1.y, p1.z });
            }
        }
        mesh.setVertices(v, VertexLayout().floats(3));
    }
    void draw(glm::ivec3 blockPos, const glm::mat4 &view, const glm::mat4 &proj);
private:
    Shader shader;
    Mesh mesh;

    float thickness;
};

#endif //OPENGL_TEST_BLOCKOUTLINE_H
