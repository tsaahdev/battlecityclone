#version 460 core

layout (location = 0) in vec2 vPos;

const int MAX_TEXTURES = 1024; // for now
layout (std140, binding = 0) uniform Textures {
    vec4 textureCoords[MAX_TEXTURES];
};

const int MAX_QUADS = 1024; // TODO:  refactor
struct QuadData {
    vec4 tilePositionAndSize;
    vec4 color;
    vec4 textureIdsAndZ;
};
layout (std140, binding = 1) uniform QuadDataBlock {
    QuadData quadData[MAX_QUADS];
};

uniform int width;
uniform int height;

out vec2 texCoord;
out flat int useColor;
out vec4 quadColor;

vec2 getTexCoord(uint subTextureId) {
    vec4 uv = textureCoords[subTextureId];
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
const float scale = round(height / 13.5f);
const vec2 mapSize = vec2(26);
// const f32 verticalTileCount = mapSize + 2
const vec2 mapCenterOffset = 0.5f * mapSize;

void main() {
    // vec2 aspectRatio = vec2(width, height) / float(height);

    const QuadData quad = quadData[gl_InstanceID];
    const vec2 tilePosition = quad.tilePositionAndSize.xy;
    const vec2 tileSize = quad.tilePositionAndSize.zw;
    const vec4 color = quad.color;
    const uint textureId = floatBitsToUint(quad.textureIdsAndZ.x);
    const uint subTextureId = floatBitsToUint(quad.textureIdsAndZ.y);
    const float z = quad.textureIdsAndZ.z;
    useColor = subTextureId == 0 ? 1 : 0;
    quadColor = color;

    const vec2 vertPos = (vPos + 0.5f) * tileSize + tilePosition - mapCenterOffset;
    // gl_Position = vec4(vertPos * scale, 0, 1) / vec4(aspectRatio, 1, 1);
    gl_Position = vec4(vertPos * scale, 0, 1) / vec4(width, height, 1, 1);

    texCoord = getTexCoord(subTextureId);
}