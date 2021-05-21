#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 2) in vec2 aTexCoord;

uniform mat4 transform;
uniform mat4 modelTransform;

out vec2 texCoord;
out vec3 position;

void main() {
    gl_Position = transform * vec4(aPos, 1.0);
    texCoord = aTexCoord;
    position = (modelTransform * vec4(aPos, 1.0)).xyz;
}