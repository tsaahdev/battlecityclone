#version 460 core

layout (location = 0) in vec2 vPos;
layout (location = 1) in vec2 qPos;
layout (location = 2) in float qSize;
layout (location = 3) in uint qTexture;
layout (location = 4) in uint qSubtexture;

const int MAX_TEXTURES = 1024; // for now
layout (std140, binding = 0) uniform Textures {
    vec4 textureCoords[MAX_TEXTURES];
};

uniform int width;
uniform int height;

out vec2 texCoord;

vec2 getTexCoord() {
    vec4 uv = textureCoords[qSubtexture];
    if (gl_VertexID == 0) {
        return uv.xy;
    } else if (gl_VertexID == 1) {
        return uv.zy;
    } else if (gl_VertexID == 2) {
        return uv.zw;
    } else if (gl_VertexID == 3) {
        return uv.xw;
    }
}

void main() {
    vec2 aspectRatio = vec2(width, height) / float(height);
    vec2 vertPos = qPos - vec2(13, 13) + vPos * qSize - vec2(0.5f) * (qSize - 1);
    gl_Position = vec4(vertPos / 14.0f, 0, 1) / vec4(aspectRatio, 1, 1);
    texCoord = getTexCoord();
}