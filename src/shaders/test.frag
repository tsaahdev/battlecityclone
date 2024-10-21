#version 460 core

in vec2 texCoord;
in flat int useColor;
in vec4 quadColor;

out vec4 FragColor;

uniform sampler2D texture0;

float average(vec3 color) {
    return (color.r + color.g + color.b) / 3.0f;
}

void main() {
    if (useColor == 1) {
        FragColor = quadColor;
    } else {
        vec4 color = texture(texture0, texCoord);
        if (average(color.rgb) < 1 / 255.0f) {
            discard;
        }
        FragColor = color;
    }
}