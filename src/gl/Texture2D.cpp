#include "Texture2D.h"

#include "GLFW/glfw3.h"

#define STB_IMAGE_IMPLEMENTATION
#include <iostream>

#include "../util/stb_image.h"

Texture2D & Texture2D::operator=(Texture2D &&obj) noexcept {
    if (this != &obj) {
        destroy();
        id = obj.id;
        obj.id = 0;
    }
    return *this;
}

bool Texture2D::load(const std::string &path, bool nearest) {
    int width, height, channels;
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
    if (!data) {
        std::cerr << "Failed to load texture: " << path << std::endl;
        return false;
    }

    GLenum format;
    GLint internalFormat;
    switch (channels) {
        case 1: format = GL_RED; internalFormat = GL_R8; break;
        case 2: format = GL_RG; internalFormat = GL_RG8; break;
        case 3: format = GL_RGB; internalFormat = GL_RGB8; break;
        default: format = GL_RGBA; internalFormat = GL_RGBA8; break;
    }

    if (!id) glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, nearest ? GL_NEAREST_MIPMAP_LINEAR : GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, nearest ? GL_NEAREST : GL_LINEAR);
    return true;
}

void Texture2D::bind(unsigned int unit) const {
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, id);
}

void Texture2D::destroy() {
    if (id && glfwGetCurrentContext()) glDeleteTextures(1, &id);
    id = 0;
}

GLuint Texture2D::getId() const {
    return id;
}
