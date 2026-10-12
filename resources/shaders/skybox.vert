#version 430 core

out vec2 ndc;

void main() {
    vec2 p = vec2((gl_VertexID << 1) & 2, gl_VertexID & 2); // (0,0) (2,0) (0,2)
    gl_Position = vec4(p * 2.0 - 1.0, 1.0, 1.0);
    ndc = p * 2.0 - 1.0;
}