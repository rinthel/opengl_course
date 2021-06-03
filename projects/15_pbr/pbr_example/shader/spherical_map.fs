#version 330 core
out vec4 fragColor;
in vec3 localPos;
uniform sampler2D tex;
uniform float exposure;

const vec2 invPi = vec2(0.1591549, 0.3183098862);
vec2 SampleSphericalMap(vec3 v) {
    return vec2(atan(v.z, v.x), asin(v.y)) * invPi + 0.5;
}

void main() {
    vec2 uv = SampleSphericalMap(normalize(localPos)); // normalize
    vec3 color = texture(tex, uv).rgb;
    fragColor = vec4(color, 1.0);
}
