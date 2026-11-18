#version 430 core

layout (location = 0) in uint a_data;
layout (location = 1) in vec3 a_chunkOffset;

uniform mat4 u_view;
uniform mat4 u_proj;

out vec3 normal;
out vec3 fragPos;
out vec2 texCoord;
out float ao;
flat out int layer;

// Must match Direction.h !!!
const vec3 NORMALS[6] = vec3[6](
        vec3(-1, 0, 0), vec3(1, 0, 0), // LEFT, RIGHT
        vec3(0, 1, 0), vec3(0, -1, 0), // UP, DOWN
        vec3(0, 0, -1), vec3(0, 0, 1) // BACK, FRONT
);

const vec2 UVS[4] = vec2[4](vec2(0), vec2(1, 0), vec2(1), vec2(0, 1));

void main() {
    vec3 local = vec3(a_data & 31u, (a_data >> 5) & 31u, (a_data >> 10) & 31u);
    uint face = (a_data >> 15) & 7u;
    uint corner = (a_data >> 18) & 3u;
    float unpackedLayer = float((a_data >> 20) & 255u);
    float unpackedAo = float((a_data >> 28) & 3u);

    vec3 pos = local + a_chunkOffset;
    vec3 unpackedNormal = NORMALS[face];
    vec2 uv = UVS[corner];

    gl_Position = u_proj * u_view * vec4(pos, 1.0);

    normal = unpackedNormal;
    fragPos = pos;
    texCoord = uv;
    layer = int(unpackedLayer + 0.5);
    ao = unpackedAo;
}