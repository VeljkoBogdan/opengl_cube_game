#version 430 core

in vec2 uv;
out vec4 fragColor;

uniform sampler2D u_scene;
uniform vec2 u_texelSize;
uniform bool u_enabled;

#define FXAA_REDUCE_MIN (1.0 / 256.0)
#define FXAA_REDUCE_MUL (1.0 / 4.0)
#define FXAA_SPAN_MAX 4.0

float luma(vec3 c) {
    return dot(c, vec3(0.299, 0.587, 0.114));
}

void main() {
    if (!u_enabled) {
        fragColor = texture(u_scene, uv);
        return;
    }

    vec3 rgbNW = texture(u_scene, uv + vec2(-1.0, -1.0) * u_texelSize).rgb;
    vec3 rgbNE = texture(u_scene, uv + vec2(1.0, -1.0) * u_texelSize).rgb;
    vec3 rgbSW = texture(u_scene, uv + vec2(-1.0, 1.0) * u_texelSize).rgb;
    vec3 rgbSE = texture(u_scene, uv + vec2(1.0, 1.0) * u_texelSize).rgb;
    vec3 rgbM = texture(u_scene, uv).rgb;

    float lumaNW = luma(rgbNW);
    float lumaNE = luma(rgbNE);
    float lumaSW = luma(rgbSW);
    float lumaSE = luma(rgbSE);
    float lumaM = luma(rgbM);

    float lumaMin = min(lumaM, min(min(lumaNW, lumaNE), min(lumaSW, lumaSE)));
    float lumaMax = max(lumaM, max(max(lumaNW, lumaNE), max(lumaSW, lumaSE)));

    // direction of the edge, perpendicular to the luma gradient
    vec2 dir;
    dir.x = -((lumaNW + lumaNE) - (lumaSW + lumaSE));
    dir.y = ((lumaNW + lumaSW) - (lumaNE + lumaSE));

    float dirReduce = max((lumaNW + lumaNE + lumaSW + lumaSE) * 0.25 * FXAA_REDUCE_MUL, FXAA_REDUCE_MIN);
    float rcpDirMin = 1.0 / (min(abs(dir.x), abs(dir.y)) + dirReduce);
    dir = clamp(dir * rcpDirMin, vec2(-FXAA_SPAN_MAX), vec2(FXAA_SPAN_MAX)) * u_texelSize;

    // blur along the edge
    vec3 rgbA = 0.5 * (
    texture(u_scene, uv + dir * (1.0 / 3.0 - 0.5)).rgb +
    texture(u_scene, uv + dir * (2.0 / 3.0 - 0.5)).rgb);
    vec3 rgbB = rgbA * 0.5 + 0.25 * (
    texture(u_scene, uv + dir * -0.5).rgb +
    texture(u_scene, uv + dir * 0.5).rgb);

    float lumaB = luma(rgbB);
    fragColor = (lumaB < lumaMin || lumaB > lumaMax) ? vec4(rgbA, 1.0) : vec4(rgbB, 1.0);
}
