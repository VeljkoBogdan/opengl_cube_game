#version 330 core

in vec3 normal;
in vec3 fragPos;
in vec2 texCoord;

out vec4 fragColor;

uniform vec3 u_ambient;
uniform vec3 u_diffuse;
uniform vec3 u_lightPos;
uniform sampler2D u_texture;

void main() {
    vec3 lightDir = normalize(u_lightPos - fragPos);
    float diff = max(dot(normalize(normal), lightDir), 0.0f);
    vec3 diffuse = u_diffuse * diff;
    vec3 lightResult = u_ambient + diffuse;

    fragColor = texture(u_texture, texCoord) * vec4(lightResult, 1.0);
}