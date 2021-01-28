#version 330 core
in vec3 normal;
in vec2 texCoord;
out vec4 fragColor;

uniform vec3 lightColor;
uniform vec3 objectColor;
uniform float ambientStrength;

void main() {
    vec3 ambient = ambientStrength * lightColor;
    vec3 result = ambient * objectColor;
    fragColor = vec4(result, 1.0);
}