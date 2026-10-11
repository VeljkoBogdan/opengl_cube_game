#version 430 core

layout (location = 0) in vec2 a_pos;

uniform float u_width;
uniform float u_height;

void main() {
    gl_Position = vec4(a_pos.x / (u_width * 0.5), a_pos.y / (u_height * 0.5), 0.0, 1.0);
}