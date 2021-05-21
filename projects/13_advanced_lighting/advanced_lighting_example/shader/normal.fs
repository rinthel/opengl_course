#version 330 core

in vec2 texCoord;
in vec3 position;
out vec4 fragColor;

uniform vec3 viewPos;
uniform vec3 lightPos;

uniform sampler2D diffuse;
uniform sampler2D normalMap;

void main() {
    vec3 texColor = texture(diffuse, texCoord).xyz;
    vec3 pixelNorm = normalize(texture(normalMap, texCoord).xyz * 2.0 - 1.0);
    vec3 ambient = texColor * 0.2;

    vec3 lightDir = normalize(lightPos - position);
    float diff = max(dot(pixelNorm, lightDir), 0.0);
    vec3 diffuse = diff * texColor * 0.8;

    vec3 viewDir = normalize(viewPos - position);
    vec3 reflectDir = reflect(-lightDir, pixelNorm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = spec * vec3(0.5);

    fragColor = vec4(ambient + diffuse + specular, 1.0);
}