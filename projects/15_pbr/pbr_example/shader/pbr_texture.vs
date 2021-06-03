#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aTangent;

uniform mat4 transform;
uniform mat4 modelTransform;

out vec3 fragPos;
out vec2 texCoord;
out mat3 TBN;

void main() {
    gl_Position = transform * vec4(aPos, 1.0);
    fragPos = (modelTransform * vec4(aPos, 1.0)).xyz;
    texCoord = aTexCoord;

    mat4 invTransModelTransform = transpose(inverse(modelTransform));
    vec3 normal = normalize((invTransModelTransform * vec4(aNormal, 0.0)).xyz);
    vec3 tangent = normalize((invTransModelTransform * vec4(aTangent, 0.0)).xyz);
    vec3 binormal = cross(normal, tangent);
    TBN = mat3(tangent, binormal, normal);
}