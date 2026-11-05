#version 430 core

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_texCoord;
layout (location = 3) in float a_layer;
layout (location = 4) in float a_ao;
layout (location = 5) in vec3 a_chunkOffset;

uniform mat4 u_view;
uniform mat4 u_proj;

out vec3 normal;
out vec3 fragPos;
out vec2 texCoord;
out float ao;
flat out int layer;

void main() {
    vec3 worldPos = a_pos + a_chunkOffset;
    gl_Position = u_proj * u_view * vec4(worldPos, 1.0);

    normal = a_normal;
    fragPos = worldPos;
    texCoord = a_texCoord;
    layer = int(a_layer + 0.5);
    ao = a_ao;
}