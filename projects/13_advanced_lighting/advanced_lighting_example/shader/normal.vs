#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aTangent;

uniform mat4 transform;
uniform mat4 modelTransform;

out vec2 texCoord;
out vec3 position;
out vec3 normal;
out vec3 tangent;

void main() {
    gl_Position = transform * vec4(aPos, 1.0);
    texCoord = aTexCoord;
    position = (modelTransform * vec4(aPos, 1.0)).xyz;

    mat4 invTransModelTransform = transpose(inverse(modelTransform));
    normal = (invTransModelTransform * vec4(aNormal, 0.0)).xyz;
    tangent = (invTransModelTransform * vec4(aTangent, 0.0)).xyz;
}