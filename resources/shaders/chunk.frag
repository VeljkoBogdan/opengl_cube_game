#version 430 core

in vec3 normal;
in vec3 fragPos;
in vec2 texCoord;
flat in int layer;

out vec4 fragColor;

uniform vec3 u_ambient;
uniform vec3 u_sunColor;
uniform sampler2DArray u_texture;
uniform vec3 u_sunDir;
uniform vec3 u_fogColor;
uniform vec3 u_zenithColor;
uniform vec3 u_camPos;
uniform vec2 u_fog;

void main() {
    float daylight = smoothstep(-0.1, 0.2, u_sunDir.y);
    float diff = max(dot(normalize(normal), u_sunDir), 0.0f) * daylight;
    // sky color impacts ambient light
    vec3 ambient = mix(u_ambient, u_zenithColor, 0.3) * mix(0.3, 1.0, daylight);
    ambient = max(ambient, vec3(0.05, 0.06, 0.12));
    // final light result
    vec3 lightResult = ambient + u_sunColor * diff * 0.8;

    // fog on distant chunks
    float fog = smoothstep(u_fog.x, u_fog.y, length(fragPos - u_camPos));

    // apply to textures
    fragColor = texture(u_texture, vec3(texCoord, float(layer))) * vec4(lightResult, 1.0);
    fragColor.rgb = mix(fragColor.rgb, u_fogColor, fog);
}