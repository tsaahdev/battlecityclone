#pragma once

#include "e_logger.h"

#include "e_filesystem.h"

// #define STB_IMAGE_IMPLEMENTATION
// #include "stb_image.h"

#include "e_myson_parser.h"

#include <string_view>

namespace base {

struct TextureManager {
    
};

// b8 loadImageFile(const char* filepath) {
    // i32 x, y, n;
    // std::make_unique<u8[], stbi_image_free>(stbi_load(filename, &x, &y, &n, 0));
// }

enum class SpriteType: u8 {
    MAP_TILES,
    ANIMATIONS,
    PICKUPS,
    TANKS
};

enum class SpriteTag: u8 {
    WALL,
    SMALL,
    BIG,
    BRICK,
    CONCRETE,
    BUSH,
    SNOW,
    BIRD,
    FLAG,
    WATER,
    SPAWN,
    STAR,
    GRENADE,
    TANK,
    GUN,
    HELMET,
    CLOCK,
    SHOVEL,
    YELLOW,
    GREEN,
    WHITE,
    RED,
    UP,
    DOWN,
    LEFT,
    RIGHT,
    PLAYER1,
    PLAYER2,
    TYPE_0,
    TYPE_1,
    TYPE_2,
    TYPE_3,
    TYPE_4,
    TYPE_5,
    TYPE_6,
    TYPE_7,
};

struct Frame {
    u32 x{ 0 };
    u32 y{ 0 };
    u32 w{ 0 };
    u32 h{ 0 };
};

struct Sprite {
    std::vector<Frame> frames;
};

struct TileSet {
    u8 version{ 0 };
    std::unordered_map<SpriteType, std::vector<Sprite>> sprites;
};

b8 loadTileSet(const char* filepath) {
    const auto size = filesystem::getFileSize(filepath);
    if (size == 0) { E_PRINT("Oops failed to load tileset, file size is 0"); return false; }
    u8* data = static_cast<u8*>(malloc(size));

    if (!filesystem::readEntireFile(filepath, data, size)) { E_PRINT("Oops failed to load tileset"); return false; }


    StringView view(data, size);
    const auto doc = myson::parseDocument(view);
    const auto version = doc.values.at("version").toU32();
    const auto sprites = doc.values.at("sprites").toObject();
    const auto map_tiles = sprites.values.at("map_tiles").toArray();
    for (const auto& tile: map_tiles.values) {
        const auto tileObject = tile.toObject();
        const auto tagsArray = tileObject.values.at("tags").toArray();
        for (const auto& tag: tagsArray.values) {
            const auto tagString = tag.toString();
            E_PRINT("TAG %s", tagString.toStdString().c_str());
        }
    }

    


    return true;
}

} // namespace base