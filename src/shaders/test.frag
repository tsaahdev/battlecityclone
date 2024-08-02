#version 460 core

in vec2 texCoord;

out vec4 FragColor;

uniform sampler2D texture0;

float average(vec3 color) {
    return (color.r + color.g + color.b) / 3.0f;
}

void main() {
    vec4 color = texture(texture0, texCoord);
    if (average(color.rgb) < 0.1) {
        discard;
    }
    FragColor = color;
}