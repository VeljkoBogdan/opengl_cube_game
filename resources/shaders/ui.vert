#version 430 core

layout(location = 0) in vec2 a_pos;
layout(location = 1) in vec2 a_uv;
layout(location = 2) in vec4 a_color;
layout(location = 3) in float a_layer;

uniform mat4 u_proj;

out vec2 uv;
out vec4 color;
flat out float layer;

void main() {
    gl_Position = u_proj * vec4(a_pos, 0.0, 1.0);
    uv = a_uv;
    color = a_color;
    layer = a_layer;
}