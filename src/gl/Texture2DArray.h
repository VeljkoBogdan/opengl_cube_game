#ifndef TEXTURE2DARRAY_H
#define TEXTURE2DARRAY_H

#include <string>
#include <vector>
#include <glad/glad.h>

class Texture2DArray {
public:
    Texture2DArray() = default;
    ~Texture2DArray() { destroy(); }

    Texture2DArray(const Texture2DArray&) = delete;
    Texture2DArray& operator=(const Texture2DArray&) = delete;
    Texture2DArray(Texture2DArray&& obj) noexcept
        : id(obj.id), width(obj.width), height(obj.height), layerCount(obj.layerCount) {
        obj.id = 0;
        obj.width = obj.height = obj.layerCount = 0;
    }
    Texture2DArray& operator=(Texture2DArray&& obj) noexcept;

    bool load(const std::vector<std::string>& paths, bool nearest = true);
    void bind(unsigned int unit = 0) const;

    [[nodiscard]] GLuint getId() const { return id; }
    [[nodiscard]] GLsizei getLayerCount() const { return layerCount; }

private:
    void destroy();
    GLuint id = 0;
    GLsizei width = 0, height = 0, layerCount = 0;
};

#endif //TEXTURE2DARRAY_H