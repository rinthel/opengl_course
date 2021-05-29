#version 330 core

out vec4 fragColor;
in vec2 texCoord;
uniform sampler2D tex;

void main() {
    vec2 texelSize = 1.0 / vec2(textureSize(tex, 0));
    vec4 result = vec4(0.0);
    for (int x = -2; x <= 2; ++x) {
        for (int y = -2; y <= 2; ++y) {
            vec2 offset = vec2(float(x), float(y)) * texelSize;
            result += texture(tex, texCoord + offset);
        }
    }
    fragColor = result / 25.0;
}