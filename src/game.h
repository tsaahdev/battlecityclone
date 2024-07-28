#pragma once

#include "common.h"

namespace base::game {

enum class TileType: u8 {
    Empty = 0,
    Brick = 1,

    Concrete = 2,
    Water = 3,
    Grass = 4,
    Ice = 5,
    Forest = 6,
    Base = 7,
    BaseDestroyed = 8,
    Eagle = 9,
    EagleDestroyed = 10,
}; // lol gpt knows i am going for battle city...

const c8* map = R"(
0000000000000
0101010101010
0101010101010
0101010101010
0101010101010
0000001000000
0101010101010
0101010101010
0101010101010
0000001000000
0000000000000
0000011100000
0000019100000
)";
} // namespace base::game