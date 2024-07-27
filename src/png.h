#pragma once

#include "common.h"

namespace base::png {

struct Png {
    u32 width;
    u32 height;
    u8* data;
};

inline b8 load(u8* fileData, size_t fileSize, Png data) {
    const u8 header[8] = { 137, 80, 78, 71, 13, 10, 26, 10 };
    if (::memcmp(fileData, header, 8) != 0) {
        E_PRINT("Invalid PNG header.");
        return false;
    }
    // ill go sleep
}

} // namespace base::png