#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

uniform mat4 transform;
uniform mat4 modelTransform;

out vec3 fragPos;
out vec3 normal;
out vec2 texCoord;

void main() {
    gl_Position = transform * vec4(aPos, 1.0);
    fragPos = (modelTransform * vec4(aPos, 1.0)).xyz;
    normal = (transpose(inverse(modelTransform)) * vec4(aNormal, 0.0)).xyz;
    texCoord = aTexCoord;
}