#ifndef OPENGL_TEST_BLOCKOUTLINE_H
#define OPENGL_TEST_BLOCKOUTLINE_H
#include "../shader.h"
#include "../gl/Mesh.h"

class BlockOutline {
public:
    BlockOutline(const char* vertPath, const char* fragPath, float thickness = 2.0f)
: shader(vertPath, fragPath), thickness(thickness) {
        constexpr float a = 0.502f;
        static const float corners[6][2] = {{0,-1}, {1,1}, {0,1}, {0,-1}, {1,-1}, {1,1}};

        std::vector<float> v;
        for (int axis = 0; axis < 3; axis++) {
            int u = (axis + 1) % 3;
            int w = (axis + 2) % 3;
            for (int su = -1; su <= 1; su += 2)
                for (int sw = -1; sw <= 1; sw += 2) {
                    glm::vec3 p0, p1;
                    p0[axis] = -a;
                    p1[axis] = a;
                    p0[u] = p1[u] = su * a;
                    p0[w] = p1[w] = sw * a;
                    for (const auto& c : corners)
                        v.insert(v.end(), { p0.x, p0.y, p0.z, p1.x, p1.y, p1.z, c[0], c[1] });
                }
        }
        mesh.setVertices(v, VertexLayout().floats(3).floats(3).floats(1).floats(1));
    }
    void draw(glm::ivec3 blockPos, const glm::mat4 &view, const glm::mat4 &proj, int fbWidth, int fbHeight);
private:
    Shader shader;
    Mesh mesh;

    float thickness;
};

#endif //OPENGL_TEST_BLOCKOUTLINE_H
