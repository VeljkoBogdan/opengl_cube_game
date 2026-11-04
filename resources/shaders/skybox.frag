#version 430 core

in vec2 ndc;

uniform mat4 u_invViewProj;
uniform vec3 u_zenithColor;
uniform vec3 u_horizonColor;
uniform vec3 u_sunColor;
uniform vec3 u_sunDir;
uniform float u_horizonOffset;
uniform float u_gradientPower;

out vec4 fragColor;

void main() {
    vec4 w = u_invViewProj * vec4(ndc, 1.0, 1.0);
    vec3 dir = normalize(w.xyz / w.w);

    // gradient
    float t = clamp((dir.y - u_horizonOffset) / (1.0 - u_horizonOffset), 0.0, 1.0);
    vec3 sky = mix(u_horizonColor, u_zenithColor, pow(t, u_gradientPower));

    float c = dot(dir, u_sunDir);
    float visible = smoothstep(-0.05, 0.02, dir.y);
    float disc = smoothstep(0.9994, 0.9998, c) * visible;
    float glow = pow(max(c, 0.0), 32.0) * 0.5;

    sky += u_sunColor * glow;
    sky = mix(sky, u_sunColor, disc);

    fragColor = vec4(sky, 1.0);
}