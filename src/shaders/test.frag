#version 460 core

in vec2 texCoord;

out vec4 FragColor;

uniform sampler2D texture0;

float average(vec3 color) {
    return (color.r + color.g + color.b) / 3.0f;
}

void main() {
    // FragColor = vec4(1, 0, 1, 1);
    vec4 color = texture(texture0, texCoord);
    if (average(color.rgb) < 1 / 255.0f) {
        discard;
    }
    FragColor = color;
}