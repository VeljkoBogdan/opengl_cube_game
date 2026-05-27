#ifndef TIMER_H
#define TIMER_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>

struct Timer {
    const char* name;
    double start;

    Timer(const char* name) : name(name), start(glfwGetTime()) {}

    ~Timer() {
        double elapsed = (glfwGetTime() - start) * 1000.0;
        if (elapsed > 5.0) printf("[%s] %.3f ms\n", name, elapsed);
    }
};

#endif