#version 430 core

in vec2 uv;
in vec4 color;
flat in float layer;

uniform sampler2DArray u_tex;

out vec4 fragColor;

void main() {
    vec4 c = color;
    if (layer >= 0.0) c *= texture(u_tex, vec3(uv, layer));
    fragColor = c;
}