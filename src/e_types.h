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

    Vector2()
        : x{}, y{}{}
    template <class V>
    Vector2(V v = {})
        : x{ static_cast<T>(v) }, y{ static_cast<T>(v) }{}
    template <class X, class Y>
    Vector2(X x, Y y)
        : x{ static_cast<T>(x) }, y{ static_cast<T>(y) }{}
    template <class V>
    Vector2(const Vector2<V>& o)
        : x{ static_cast<T>(o.x) }, y{ static_cast<T>(o.y) }{}
    template <class V>
    Vector2(Vector2<V>&& o)
        : x{ static_cast<T>(o.x) }, y{ static_cast<T>(o.y) }{}
    template <class V>
    Vector2& operator=(const Vector2<V>& o) {
        x = static_cast<T>(o.x);
        y = static_cast<T>(o.y);
        return *this;
    }
    template <class V>
    Vector2& operator=(Vector2<V>&& o) {
        x = static_cast<T>(o.x);
        y = static_cast<T>(o.y);
        return *this;
    }

    Vector2& operator+=(const Vector2<T>& rhs) {
        x += rhs.x;
        y += rhs.y;
        return *this;
    }

    Vector2& operator+(const Vector2<T>& rhs) {
        return { x + rhs.x, y  + rhs.y };
    }
};

template <class T>
Vector2<T> operator*(const Vector2<T>& l, f32 r) {
    return { l.x * r, l.y * r };
}

template <class T>
Vector2<T> operator*(f32 l, const Vector2<T>& r) {
    return r * l;
}

using v2 = Vector2<f32>;
using v2i = Vector2<i32>;

template <class T>
union Vector3 {
    struct {
        T x;
        T y;
        T z;
    };
    struct {
        T r;
        T g;
        T b;
    };
    T data[3];

    Vector3()
        : x{}, y{}, z{}{}
    template <class V>
    Vector3(V v)
        : x{ static_cast<T>(v) }, y{ static_cast<T>(v) }, z{ static_cast<T>(v) }{}
    template <class X, class Y, class Z>
    Vector3(X ax, Y ay, Z az = {})
        : x{ static_cast<T>(ax) }, y{ static_cast<T>(ay) }, z{ static_cast<T>(az) }{}
    template <class V, class Z>
    Vector3(const Vector2<V>& v, Z az = {})
        : x{ static_cast<T>(v.x) }, y{ static_cast<T>(v.y) }, z{ static_cast<T>(az) }{}
    template <class V, class Z>
    Vector3(Vector2<V>&& v, Z az = {})
        : x{ static_cast<T>(v.x) }, y{ static_cast<T>(v.y) }, z{ static_cast<T>(az) }{}
    template <class V>
    Vector3(const Vector3<V>& v)
        : x{ static_cast<T>(v.x) }, y{ static_cast<T>(v.y) }, z{ static_cast<T>(v.z) }{}
    template <class V>
    Vector3(Vector2<V>&& v)
        : x{ static_cast<T>(v.x) }, y{ static_cast<T>(v.y) }, z{ static_cast<T>(v.z) }{}

    template <class V>
    Vector3& operator=(const Vector2<V>& v) {
        x = static_cast<T>(v.x); y = static_cast<T>(v.y); z = {};
    }
    template <class V>
    Vector3& operator=(Vector2<V>&& v) {
        x = static_cast<T>(v.x); y = static_cast<T>(v.y); z = {};
    }
    template <class V>
    Vector3& operator=(const Vector3<V>& v) {
        x = static_cast<T>(v.x); y = static_cast<T>(v.y); z = static_cast<T>(v.z);
    }
    template <class V>
    Vector3& operator=(Vector3<V>&& v) {
        x = static_cast<T>(v.x); y = static_cast<T>(v.y); z = static_cast<T>(v.z);
    }
};

using v3 = Vector3<f32>;

template <class T>
union Vector4 {
    struct {
        T x;
        T y;
        T z;
        T w;
    };
    struct {
        T r;
        T g;
        T b;
        T a;
    };
    T data[4];

    Vector4()
        : x{}, y{}, z{}, w{}{}
    template <class V>
    Vector4(V v)
        : x{ static_cast<T>(v) }, y{ static_cast<T>(v) }, z{ static_cast<T>(v)}, w{ static_cast<T>(v) }{}
    template <class X, class Y, class Z, class W>
    Vector4(X ax, Y ay, Z az = {}, W aw = {})
        : x{ static_cast<T>(ax) }, y{ static_cast<T>(ay) }, z{ static_cast<T>(az) }, w{ static_cast<T>(aw) }{}
    template <class V, class Z, class W>
    Vector4(const Vector2<V>& v, Z az = {}, W aw = {})
        : x{ static_cast<T>(v.x) }, y{ static_cast<T>(v.y) }, z{ static_cast<T>(az) }, w{ static_cast<T>(aw) }{}
    template <class V, class Z, class W>
    Vector4(Vector2<V>&& v, Z az = {}, W aw = {})
        : x{ static_cast<T>(v.x) }, y{ static_cast<T>(v.y) }, z{ static_cast<T>(az) }, w{ static_cast<T>(aw) }{}
    template <class V, class W>
    Vector4(const Vector3<V>& v, W aw = {})
        : x{ static_cast<T>(v.x) }, y{ static_cast<T>(v.y) }, z{ static_cast<T>(v.z) }, w{ static_cast<T>(aw) }{}
    template <class V, class W>
    Vector4(Vector3<V>&& v, W aw = {})
        : x{ static_cast<T>(v.x) }, y{ static_cast<T>(v.y) }, z{ static_cast<T>(v.z) }, w{ static_cast<T>(aw) }{}
    template <class V>
    Vector4(const Vector4<V>& v)
        : x{ static_cast<T>(v.x) }, y{ static_cast<T>(v.y) }, z{ static_cast<T>(v.z) }, w{ static_cast<T>(v.w) }{}
    template <class V>
    Vector4(Vector4<V>&& v)
        : x{ static_cast<T>(v.x) }, y{ static_cast<T>(v.y) }, z{ static_cast<T>(v.z) }, w{ static_cast<T>(v.w) }{}

    template <class V>
    Vector4& operator=(const Vector2<V>& v) {
        x = static_cast<T>(v.x); y = static_cast<T>(v.y); z = {}; w = {};
    }
    template <class V>
    Vector4& operator=(Vector2<V>&& v) {
        x = static_cast<T>(v.x); y = static_cast<T>(v.y); z = {}; w = {};
    }
    template <class V>
    Vector4& operator=(const Vector3<V>& v) {
        x = static_cast<T>(v.x); y = static_cast<T>(v.y); z = static_cast<T>(v.z); w = {};
    }
    template <class V>
    Vector4& operator=(Vector3<V>&& v) {
        x = static_cast<T>(v.x); y = static_cast<T>(v.y); z = static_cast<T>(v.z); w = {};
    }
    template <class V>
    Vector4& operator=(const Vector4<V>& v) {
        x = static_cast<T>(v.x); y = static_cast<T>(v.y); z = static_cast<T>(v.z); w = static_cast<T>(v.w);
    }
    template <class V>
    Vector4& operator=(Vector4<V>&& v) {
        x = static_cast<T>(v.x); y = static_cast<T>(v.y); z = static_cast<T>(v.z); w = static_cast<T>(v.w);
    }
};

using v4 = Vector4<f32>;

} // namespace base