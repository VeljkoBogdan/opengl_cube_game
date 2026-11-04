#include "Texture2DArray.h"

#include <GLFW/glfw3.h>
#include <iostream>

#include "../util/stb_image.h" // NOTE: no STB_IMAGE_IMPLEMENTATION here, it's already in Texture2D.cpp

Texture2DArray& Texture2DArray::operator=(Texture2DArray&& obj) noexcept {
    if (this != &obj) {
        destroy();
        id = obj.id;
        width = obj.width;
        height = obj.height;
        layerCount = obj.layerCount;
        obj.id = 0;
        obj.width = obj.height = obj.layerCount = 0;
    }
    return *this;
}

bool Texture2DArray::load(const std::vector<std::string>& paths, bool nearest) {
    if (paths.empty()) {
        std::cerr << "Texture2DArray::load called with no paths\n";
        return false;
    }

    const auto layers = static_cast<GLsizei>(paths.size());
    GLsizei w = 0, h = 0;

    std::vector<unsigned char*> images(paths.size(), nullptr);
    auto freeAll = [&] { for (auto* p : images) if (p) stbi_image_free(p); };

    stbi_set_flip_vertically_on_load(true);
    for (size_t i = 0; i < paths.size(); ++i) {
        int iw, ih, channels;
        images[i] = stbi_load(paths[i].c_str(), &iw, &ih, &channels, 4);
        if (!images[i]) {
            std::cerr << "Failed to load texture: " << paths[i] << std::endl;
            freeAll();
            return false;
        }
        if (i == 0) {
            w = iw; h = ih;
        } else if (iw != w || ih != h) {
            std::cerr << "Texture size mismatch in array: " << paths[i]
                      << " (" << iw << "x" << ih << ", expected " << w << "x" << h << ")\n";
            freeAll();
            return false;
        }
    }

    destroy();
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D_ARRAY, id);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA8, w, h, layers, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    for (GLsizei i = 0; i < layers; ++i) {
        glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, w, h, 1,
                        GL_RGBA, GL_UNSIGNED_BYTE, images[i]);
    }
    freeAll();

    glGenerateMipmap(GL_TEXTURE_2D_ARRAY);

    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER,
                    nearest ? GL_NEAREST_MIPMAP_LINEAR : GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, nearest ? GL_NEAREST : GL_LINEAR);

    width = w; height = h; layerCount = layers;
    return true;
}

void Texture2DArray::bind(unsigned int unit) const {
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D_ARRAY, id);
}

void Texture2DArray::destroy() {
    if (id && glfwGetCurrentContext()) glDeleteTextures(1, &id);
    id = 0;
}