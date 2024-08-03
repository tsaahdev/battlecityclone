#pragma once

#include "common.h"
#include "win32_opengl.h"
namespace base::map {

static const u32 MAX_TILES = 4096;
static const u32 MAX_TANKS = 64;
static const u32 MAX_BULLETS = 1024;
static const f32 TANK_SPEED_1 = 8.0f;

struct Actions {
    b8 t1_up{ false };
    b8 t1_down{ false };
    b8 t1_left{ false };
    b8 t1_right{ false };
    b8 t1_fire{ false };
    b8 t1_pause{ false };
    b8 t1_start{ false };
} actions;

enum class TileType: i8 {
    Empty = 0,
    Brick = 1,
    Concrete = 2,
    Water = 3,
    Forest = 4,
    Snow = 5,
    Bird = 6,
    Flag = 7,
};

enum class Side: u8 {
    INVALID = 0,
    Player1 = 1,
    Player2 = 2,
    Enemy = 3,
};

enum class Direction: u8 {
    Up = 0,
    Right = 1,
    Down = 2,
    Left = 3,
};

struct MapTile {
    TileType type{ TileType::Empty };
    b8 canDriveOnIt{ false };
    b8 canFlyThroughIt{ false };
};

struct Tank {
    b8 alive{ false };
    b8 driving{ false };
    v2 pos;
    Direction dir{ Direction::Up };
    i8 lives{ 3 };
};

struct Bullet {
    b8 alive{ false };
    v2 pos;
    f32 speed{ 4.0f };
    Direction dir{ Direction::Up };
    Side shotBy{ Side::INVALID };
};

struct Map {
    i32 width{ 26 };
    i32 height{ 26 };
    MapTile tiles[MAX_TILES]{};
    u32 currentTankCount{ 0 };
    Tank tanks[MAX_TANKS]{};
    Tank* player1{ nullptr };
    Tank* player2{ nullptr };
    u32 currentBulletCount{ 0 };
    Bullet bullets[MAX_BULLETS]{};
};

inline void setTile(Map& map, i32 x, i32 y, TileType type) {
    if (x < 0 || x >= map.width || y < 0 || y >= map.height) {
        return;
    }
    switch (type) {
        case TileType::Brick:
        case TileType::Concrete:
        case TileType::Bird: {
            map.tiles[y * map.width + x].canDriveOnIt = false;
            map.tiles[y * map.width + x].canFlyThroughIt = false;
        } break;
        case TileType::Forest:
        case TileType::Snow: {
            map.tiles[y * map.width + x].canDriveOnIt = true;
            map.tiles[y * map.width + x].canFlyThroughIt = true;
        } break;
        case TileType::Water:
        case TileType::Flag: {
            map.tiles[y * map.width + x].canDriveOnIt = false;
            map.tiles[y * map.width + x].canFlyThroughIt = true;
        } break;
        default: {}
    }
    map.tiles[y * map.width + x].type = type;
}

inline void createEmptyMap(Map& map, i32 width, i32 height) {
    map.width = width;
    map.height = height;
}

inline void createMap0(Map& map) {
    map.width = 26;
    map.height = 26;
    const c8* mapString =
        "00000000000000000000000000"
        "00000000000000000000000000"
        "00110011001100110011001100"
        "00110011001100110011001100"
        "00110011001100110011001100"
        "00110011001100110011001100"
        "001100110011cc110011001100"
        "001100110011cc110011001100"
        "00110011001100110011001100"
        "00110011000000000011001100"
        "00110011000000000011001100"
        "00000000001100110000000000"
        "00000000001100110000000000"
        "11001111000000000011110011"
        "cc0011110000000000111100cc"
        "00000000001100110000000000"
        "00000000001111110000000000"
        "00110011001111110011001100"
        "00110011001100110011001100"
        "00110011001100110011001100"
        "00110011001100110011001100"
        "00110011000000000011001100"
        "00110011000000000011001100"
        "00110011000111100011001100"
        "0000000000010b100000000000"
        "00000000000100100000000000";
    for (i32 y = 0; y < 26; ++y) {
        for (i32 x = 0; x < 26; ++x) {
            const c8 c = mapString[y * 26 + x];
            if (c == '\0') { break;}
            if (c == '1') {
                setTile(map, x, y, TileType::Brick);
            } else if (c == 'c') {
                setTile(map, x, y, TileType::Concrete);
            } else if (c == 'b') {
                setTile(map, x, y, TileType::Bird);
            }
        }
    }
}

inline void placeTank(Map& map, i32 x, i32 y, Side side) {
    if (map.currentTankCount >= MAX_TANKS) {
        return;
    }
    map.tanks[map.currentTankCount].alive = true;
    map.tanks[map.currentTankCount].pos = { (f32)x, (f32)y };
    map.tanks[map.currentTankCount].dir = Direction::Up;
    map.tanks[map.currentTankCount].lives = 3;
    if (side == Side::Player1) {
        map.player1 = &map.tanks[map.currentTankCount];
    } else if (side == Side::Player2) {
        map.player2 = &map.tanks[map.currentTankCount];
    }
    ++map.currentTankCount;
}

inline void placeBullet(Map& map, i32 x, i32 y, Direction dir, Side side) {
    if (map.currentBulletCount >= MAX_BULLETS) {
        return;
    }
    map.bullets[map.currentBulletCount].alive = true;
    map.bullets[map.currentBulletCount].pos = { (f32)x, (f32)y };
    map.bullets[map.currentBulletCount].dir = dir;
    map.bullets[map.currentBulletCount].shotBy = side;
    ++map.currentBulletCount;
}
inline b8 canDrive(const Map& map, f32 x, f32 y) {
    const i32 x0 = (i32)floorf(x) - 1;
    const i32 y0 = (i32)floorf(y);
    const i32 x1 = (i32)ceilf(x);
    const i32 y1 = (i32)ceilf(y) + 1;
    if (x0 < 0 || x1 >= map.width || y0 < 0 || y1 >= map.height) {
        return false;
    }
    if (map.tiles[y0 * map.width + x0].type == TileType::Empty &&
        map.tiles[y0 * map.width + x1].type == TileType::Empty &&
        map.tiles[y1 * map.width + x0].type == TileType::Empty &&
        map.tiles[y1 * map.width + x1].type == TileType::Empty) {
        return true;
    }
    if (map.tiles[y0 * map.width + x0].canDriveOnIt &&
        map.tiles[y0 * map.width + x1].canDriveOnIt &&
        map.tiles[y1 * map.width + x0].canDriveOnIt &&
        map.tiles[y1 * map.width + x1].canDriveOnIt) {
        return true;
    }
    return false;
}
inline void moveTank(Map& map, Tank& tank, Direction dir, f32 dt) {
    const auto amount = TANK_SPEED_1 * dt;
    switch (dir) {
        case Direction::Up: {
            if (canDrive(map, tank.pos.x, tank.pos.y - amount)) {
                tank.pos.y -= amount;
            } else {
                tank.pos.y = roundf(tank.pos.y);
            }
            tank.pos.x = roundf(tank.pos.x);
            tank.dir = Direction::Up;
            tank.driving = true;
        } break;
        case Direction::Down: {
            if (canDrive(map, tank.pos.x, tank.pos.y + amount)) {
                tank.pos.y += amount;
            } else {
                tank.pos.y = roundf(tank.pos.y);
            }
            tank.pos.x = roundf(tank.pos.x);
            tank.dir = Direction::Down;
            tank.driving = true;
        } break;
        case Direction::Left: {
            if (canDrive(map, tank.pos.x - amount, tank.pos.y)) {
                tank.pos.x -= amount;
            } else {
                tank.pos.x = roundf(tank.pos.x);
            }
            tank.pos.y = roundf(tank.pos.y);
            tank.dir = Direction::Left;
            tank.driving = true;
        } break;
        case Direction::Right: {
            if (canDrive(map, tank.pos.x + amount, tank.pos.y)) {
                tank.pos.x += amount;
            } else {
                tank.pos.x = roundf(tank.pos.x);
            }
            tank.pos.y = roundf(tank.pos.y);
            tank.dir = Direction::Right;
            tank.driving = true;
        } break;
    }


}

static b8 flip{ false };
inline void update(Map& map, f32 dt) {
    b8 newFlip = flip;
    static f32 accmulator{ 0.0f };
    accmulator += dt;
    static const f32 threshold = 2.0f / 60.0f;
    if (accmulator > threshold) {
        accmulator -= threshold;
        newFlip ^= true;
    }

    if (map.player1) {
        if (actions.t1_up) {
            moveTank(map, *map.player1, Direction::Up, dt);
        } else if (actions.t1_down) {
            moveTank(map, *map.player1, Direction::Down, dt);
        } else if (actions.t1_left) {
            moveTank(map, *map.player1, Direction::Left, dt);
        } else if (actions.t1_right) {
            moveTank(map, *map.player1, Direction::Right, dt);
        }
        if (map.player1->driving) {
            flip = newFlip;
        }
    }
}

inline void pushQuads(const Map& map) {
    for (i32 y = 0; y < map.height; ++y) {
        for (i32 x = 0; x < map.width; ++x) {
            const TileType type = map.tiles[y * map.width + x].type;
            if (type == TileType::Empty) { continue; }
            const f32 xf = (f32)x - 13.0f;
            const f32 yf = 13.0f - (f32)y;
            switch (type) {
                case TileType::Brick: {
                    opengl::pushQuad(opengl::SubTextureId::Brick0, { xf, yf });
                } break;
                case TileType::Concrete: {
                    opengl::pushQuad(opengl::SubTextureId::Concrete, { xf, yf });
                } break;
                case TileType::Bird: {
                    opengl::pushQuad(opengl::SubTextureId::Bird, { xf, yf }, 2);
                } break;
                default: {}
            }
        }
    }
    for (u32 i = 0; i < map.currentTankCount; ++i) {
        if (!map.tanks[i].alive) { continue; }
        const f32 xf = map.tanks[i].pos.x - 13.0f;
        const f32 yf = 13.0f - map.tanks[i].pos.y;
        opengl::SubTextureId tankSprite{ opengl::SubTextureId::Tank_0_Y_U_0 };
        switch (map.tanks[i].dir) {
            case Direction::Up: {
                if (flip) {
                    tankSprite = opengl::SubTextureId::Tank_0_Y_U_0;
                } else {
                    tankSprite = opengl::SubTextureId::Tank_0_Y_U_1;
                }
            } break;
            case Direction::Right: {
                if (flip) {
                    tankSprite = opengl::SubTextureId::Tank_0_Y_R_1;
                } else {
                    tankSprite = opengl::SubTextureId::Tank_0_Y_R_0;
                }
            } break;
            case Direction::Down: {
                if (flip) {
                    tankSprite = opengl::SubTextureId::Tank_0_Y_D_1;
                } else {
                    tankSprite = opengl::SubTextureId::Tank_0_Y_D_0;
                }
            } break;
            case Direction::Left: {
                if (flip) {
                    tankSprite = opengl::SubTextureId::Tank_0_Y_L_1;
                } else {
                    tankSprite = opengl::SubTextureId::Tank_0_Y_L_0;
                }
            } break;
        }
        opengl::pushQuad(tankSprite, { xf, yf }, 2);
    }
    for (u32 i = 0; i < map.currentBulletCount; ++i) {
        if (!map.bullets[i].alive) { continue; }
        const f32 xf = map.bullets[i].pos.x;
        const f32 yf = map.bullets[i].pos.y;
        switch (map.bullets[i].dir) {
            case Direction::Up: {
                // opengl::pushQuad(opengl::SubTextureId::Bullet_U, { xf, yf });
            } break;
            case Direction::Right: {
                // opengl::pushQuad(opengl::SubTextureId::Bullet_R, { xf, yf });
            } break;
            case Direction::Down: {
                // opengl::pushQuad(opengl::SubTextureId::Bullet_D, { xf, yf });
            } break;
            case Direction::Left: {
                // opengl::pushQuad(opengl::SubTextureId::Bullet_L, { xf, yf });
            } break;
        }
    }
}





} // namespace base::map