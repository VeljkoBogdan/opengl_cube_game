#ifndef OPENGL_TEST_TEXTURE2D_H
#define OPENGL_TEST_TEXTURE2D_H
#include <string>

#include "glad/glad.h"

class Texture2D {
public:
    Texture2D() = default;
    ~Texture2D() { destroy(); }

    Texture2D(const Texture2D&) = delete;
    Texture2D& operator=(const Texture2D&) = delete;
    Texture2D(Texture2D&& obj) noexcept : id(obj.id) {}
    Texture2D& operator=(Texture2D&& obj) noexcept;

    bool load(const std::string& path, bool nearest);
    void bind(unsigned int unit = 0) const;
    void destroy();
    GLuint getId() const;
private:
    GLuint id = 0;
};

#endif //OPENGL_TEST_TEXTURE2D_H
