#version 430 core

in vec3 normal;
in vec3 fragPos;
in vec2 texCoord;
flat in int layer;

out vec4 fragColor;

uniform vec3 u_ambient;
uniform vec3 u_diffuse;
uniform sampler2DArray u_texture;
uniform vec3 u_sunDir;
uniform vec3 u_horizonColor;
uniform vec3 u_camPos;
uniform vec2 u_fog;

void main() {
    float daylight = smoothstep(-0.1, 0.2, u_sunDir.y);
    float diff = max(dot(normalize(normal), u_sunDir), 0.0f) * daylight;
    vec3 lightResult = u_ambient * mix(0.3, 1.0, daylight) + u_diffuse * diff;

    float fog = smoothstep(u_fog.x, u_fog.y, length(fragPos - u_camPos));

    fragColor = texture(u_texture, vec3(texCoord, float(layer))) * vec4(lightResult, 1.0);
    fragColor.rgb = mix(fragColor.rgb, u_horizonColor, fog);
}