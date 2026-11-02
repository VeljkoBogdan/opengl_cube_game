#version 430 core

layout (location = 0) in vec3 a_pos;

uniform vec3 u_worldPos;
uniform mat4 u_view;
uniform mat4 u_proj;

void main() {
    gl_Position = u_proj * u_view * vec4(a_pos + u_worldPos, 1.0);
}