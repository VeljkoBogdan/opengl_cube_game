#version 430 core

in vec3 normal;
in vec3 fragPos;
in vec2 texCoord;

out vec4 fragColor;

uniform vec3 u_ambient;
uniform vec3 u_diffuse;
uniform sampler2D u_texture;
uniform vec3 u_sunDir;
uniform vec3 u_horizonColor;
uniform vec3 u_camPos;

void main() {
    float daylight = smoothstep(-0.1, 0.2, u_sunDir.y); // fading
    float diff = max(dot(normalize(normal), u_sunDir), 0.0f) * daylight;
    vec3 lightResult = u_ambient * mix(0.3, 1.0, daylight) + u_diffuse * diff;

    float fogStart = 64.0;
    float fogEnd = 96.0;
    float fog = smoothstep(fogStart, fogEnd, length(fragPos - u_camPos));

    fragColor = texture(u_texture, texCoord) * vec4(lightResult, 1.0);
    fragColor.rgb = mix(fragColor.rgb, u_horizonColor, fog);
}