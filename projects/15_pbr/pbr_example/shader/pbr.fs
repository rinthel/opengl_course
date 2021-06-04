#version 330 core
in vec3 normal;
in vec2 texCoord;
in vec3 fragPos;

out vec4 fragColor;

uniform vec3 viewPos;

struct Light {
    vec3 position;
    vec3 color;
};
const int LIGHT_COUNT = 4;
uniform Light lights[LIGHT_COUNT];

struct Material {
    vec3 albedo;
    float metallic;
    float roughness;
    float ao;
};
uniform Material material;

const float PI = 3.14159265359;

float DistributionGGX(vec3 normal, vec3 halfDir, float roughness) {
    float a = roughness * roughness;
    float a2 = a * a;
    float dotNH = max(dot(normal, halfDir), 0.0);
    float dotNH2 = dotNH * dotNH;

    float num = a2;
    float denom = (dotNH2 * (a2 - 1.0) + 1.0);
    return a2 / (PI * denom * denom);
}

float GeometrySchlickGGX(float dotNV, float roughness) {
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float num = dotNV;
    float denom = dotNV * (1.0 - k) + k;
    return num / denom;
}

float GeometrySmith(vec3 normal, vec3 viewDir, vec3 lightDir, float roughness) {
    float dotNV = max(dot(normal, viewDir), 0.0);
    float dotNL = max(dot(normal, lightDir), 0.0);
    float ggx2 = GeometrySchlickGGX(dotNV, roughness);
    float ggx1 = GeometrySchlickGGX(dotNL, roughness);
    return ggx1 * ggx2;
}

vec3 FresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

void main() {
    vec3 albedo = material.albedo;
    float metallic = material.metallic;
    float roughness = material.roughness;
    float ao = material.ao;
    vec3 fragNormal = normalize(normal);
    vec3 viewDir = normalize(viewPos - fragPos);
    float dotNV = max(dot(fragNormal, viewDir), 0.0);

    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);

    // reflectance equation
    vec3 outRadiance = vec3(0.0);
    for (int i = 0; i < LIGHT_COUNT; i++) {
        // calculate per-light radiance
        vec3 lightDir = normalize(lights[i].position - fragPos);
        vec3 halfDir = normalize(viewDir + lightDir);

        float dist = length(lights[i].position - fragPos);
        float attenuation = 1.0 / (dist * dist);
        vec3 radiance = lights[i].color * attenuation;

        // Cook-Torrance BRDF
        float ndf = DistributionGGX(fragNormal, halfDir, roughness);
        float geometry = GeometrySmith(fragNormal, viewDir, lightDir, roughness);
        vec3 fresnel = FresnelSchlick(max(dot(halfDir, viewDir), 0.0), F0);

        vec3 kS = fresnel;
        vec3 kD = 1.0 - kS;
        kD *= (1.0 - metallic);

        float dotNL = max(dot(fragNormal, lightDir), 0.0);
        vec3 numerator = ndf * geometry * fresnel;
        float denominator = 4.0 * dotNV * dotNL;
        vec3 specular = numerator / max(denominator, 0.001);

        // add to outgoing radiance Lo
        outRadiance += (kD * albedo / PI + specular) * radiance * dotNL;
    }

    vec3 ambient = vec3(0.03) * albedo * ao;
    vec3 color = ambient + outRadiance;

    // Reinhard tone mapping + gamma correction
    color = color / (color + 1.0);
    color = pow(color, vec3(1.0 / 2.2));

    fragColor = vec4(color, 1.0);
}