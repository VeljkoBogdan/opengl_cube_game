#version 430 core

layout (location = 0) in vec3 a_p0;
layout (location = 1) in vec3 a_p1;
layout (location = 2) in float a_end;
layout (location = 3) in float a_side;

uniform vec3 u_worldPos;
uniform mat4 u_view;
uniform mat4 u_proj;
uniform vec2 u_screenSize;
uniform float u_thickness;

void main() {
    mat4 vp = u_proj * u_view;
    vec4 c0 = vp * vec4(a_p0 + u_worldPos, 1.0);
    vec4 c1 = vp * vec4(a_p1 + u_worldPos, 1.0);

    float d0 = c0.z + c0.w;
    float d1 = c1.z + c1.w;
    if (d0 < 0.0 && d1 < 0.0) { gl_Position = vec4(0.0, 0.0, 2.0, 1.0); return; }
    if (d0 < 0.0) c0 = mix(c0, c1, d0 / (d0 - d1));
    else if (d1 < 0.0) c1 = mix(c1, c0, d1 / (d1 - d0));

    vec2 s0 = c0.xy / c0.w * u_screenSize * 0.5;
    vec2 s1 = c1.xy / c1.w * u_screenSize * 0.5;
    vec2 dir = s1 - s0;
    dir = length(dir) > 1e-4 ? normalize(dir) : vec2(1.0, 0.0);
    vec2 normal = vec2(-dir.y, dir.x);

    vec4 c = a_end < 0.5 ? c0 : c1;
    vec2 extend = dir * (a_end < 0.5 ? -1.0 : 1.0);
    vec2 ndc = c.xy / c.w + (normal * a_side + extend) * u_thickness / u_screenSize;

    gl_Position = vec4(ndc * c.w, c.z, c.w);
}