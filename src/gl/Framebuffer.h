#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

class Framebuffer {
public:
    Framebuffer() = default;
    ~Framebuffer() { destroy(); }

    Framebuffer(const Framebuffer&) = delete;
    Framebuffer& operator=(const Framebuffer&) = delete;
    Framebuffer(Framebuffer&&) = delete;
    Framebuffer& operator=(Framebuffer&&) = delete;

    bool resize(int width, int height) {
        if (width <= 0 || height <= 0) return false;
        if (width == width_ && height == height_ && fbo_) return true;

        destroy();
        width_ = width;
        height_ = height;

        glGenTextures(1, &color_);
        glBindTexture(GL_TEXTURE_2D, color_);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glGenRenderbuffers(1, &depth_);
        glBindRenderbuffer(GL_RENDERBUFFER, depth_);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);

        glGenFramebuffers(1, &fbo_);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_, 0);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_);

        bool ok = glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
        if (!ok) std::cerr << "Framebuffer is not complete\n";
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return ok;
    }

    void bind() const {
        glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
        glViewport(0, 0, width_, height_);
    }

    static void bindDefault(int width, int height) {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, width, height);
    }

    void bindColorTexture(unsigned int unit = 0) const {
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_2D, color_);
    }

    int width() const { return width_; }
    int height() const { return height_; }

private:
    void destroy() {
        if (glfwGetCurrentContext()) {
            if (fbo_)   glDeleteFramebuffers(1, &fbo_);
            if (color_) glDeleteTextures(1, &color_);
            if (depth_) glDeleteRenderbuffers(1, &depth_);
        }
        fbo_ = color_ = depth_ = 0;
    }

    GLuint fbo_ = 0, color_ = 0, depth_ = 0;
    int width_ = 0, height_ = 0;
};

#endif