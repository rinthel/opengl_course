#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out VS_OUT {
    vec3 fragPos;
    vec3 normal;
    vec2 texCoord;
    vec4 fragPosLight;
} vs_out;

uniform mat4 transform;
uniform mat4 modelTransform;
uniform mat4 lightTransform;

void main() {
    gl_Position = transform * vec4(aPos, 1.0);
    vs_out.fragPos = vec3(modelTransform * vec4(aPos, 1.0));
    vs_out.normal = transpose(inverse(mat3(modelTransform))) * aNormal;
    vs_out.texCoord = aTexCoord;
    vs_out.fragPosLight = lightTransform * vec4(vs_out.fragPos, 1.0);
}