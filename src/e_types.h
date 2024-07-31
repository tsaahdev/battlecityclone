#pragma once

#include <stdint.h>

namespace base {

using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;

using f32 = float;
using f64 = double;

using c8 = char;
using b8 = bool;
using b32 = int32_t;

constexpr i32 I32_MIN = -2147483647i32 - 1;
constexpr i32 I32_MAX = 2147483647i32;
constexpr u32 U32_MAX = 0xffffffffui32;

template <class T>
union Vector2 {
    struct {
        T x;
        T y;
    };
    T data[2];
};

using v2 = Vector2<f32>;
using v2i = Vector2<i32>;

union v4 {
    struct {
        f32 x;
        f32 y;
        f32 z;
        f32 w;
    };
    struct {
        f32 r;
        f32 g;
        f32 b;
        f32 a;
    };
    f32 data[4];
};

} // namespace base