#version 330 core

out float fragColor;

in vec2 texCoord;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D texNoise;

uniform mat4 view;
uniform mat4 projection;

uniform vec2 noiseScale;
uniform float radius;

const int KERNEL_SIZE = 64;
const float BIAS = 0.025;
uniform vec3 samples[KERNEL_SIZE];

void main() {
    vec4 worldPos = texture(gPosition, texCoord);
    if (worldPos.w <= 0.0f)
        discard;
    vec3 fragPos = (view * vec4(worldPos.xyz, 1.0)).xyz;
    vec3 normal = normalize((view * vec4(texture(gNormal, texCoord).xyz, 0.0)).xyz);
    vec3 randomVec = texture(texNoise, texCoord * noiseScale).xyz;

    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);

    float occlusion = 0.0;
    for (int i = 0; i < KERNEL_SIZE; i++) {
        vec3 sample = fragPos + TBN * samples[i] * radius;
        vec4 screenSample = projection * vec4(sample, 1.0);
        screenSample.xyz /= screenSample.w;
        screenSample.xyz = screenSample.xyz * 0.5 + 0.5;

        float sampleDepth = (view * texture(gPosition, screenSample.xy)).z;
        float rangeCheck = smoothstep(0.0, 1.0,
            radius / abs(fragPos.z - sampleDepth));
        occlusion += (sampleDepth >= sample.z + BIAS ? 1.0 : 0.0) * rangeCheck;
    }

    fragColor = 1.0 - occlusion / KERNEL_SIZE;
}