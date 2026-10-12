#version 430 core

in vec2 ndc;

uniform mat4 u_invViewProj;
uniform vec3 u_zenithColor;
uniform vec3 u_horizonColor;
uniform vec3 u_sunDir;

out vec4 fragColor;

void main() {
    vec4 w = u_invViewProj * vec4(ndc, 1.0, 1.0);
    vec3 dir = normalize(w.xyz / w.w);

    vec3 sky = mix(u_horizonColor, u_zenithColor, clamp(dir.y, 0.0, 1.0));

    float c = dot(dir, u_sunDir);
    float disc = smoothstep(0.9994, 0.9998, c);
    float glow = pow(max(c, 0.0), 32.0) * 0.5;

    sky += vec3(1.0, 0.9, 0.7) * glow;
    sky = mix(sky, vec3(1.0, 0.98, 0.9), disc);

    fragColor = vec4(sky, 1.0);
}
