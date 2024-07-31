#version 460 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in int iX;
layout (location = 2) in int iY;
layout (location = 3) in int iSize;
layout (location = 4) in int iTexture;
layout (location = 5) in int iSubtexture;

const int MAX_TEXTURES = 1024; // for now
layout (std140, binding = 0) uniform Textures {
    vec4 textureCoords[2 * MAX_TEXTURES];
};

out vec2 texCoord;

void main() {

    vec2 worldPos = vec2(iX, iY);
    vec2 vertPos = worldPos + aPos;
    gl_Position = vec4(vertPos / 26.0f, 0, 1);
    if (gl_VertexID == 0) {
        texCoord = textureCoords[2 * iSubtexture + 0].xy;
    } else if (gl_VertexID == 1) {
        texCoord = textureCoords[2 * iSubtexture + 0].zw;
    } else if (gl_VertexID == 2) {
        texCoord = textureCoords[2 * iSubtexture + 1].xy;
    } else if (gl_VertexID == 3) {
        texCoord = textureCoords[2 * iSubtexture + 1].zw;
    }
   
}