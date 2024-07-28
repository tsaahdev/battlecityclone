#pragma once

#include "common.h"

#include <cstdlib> // TODO: rid of this

namespace base::png {

struct Png {
    u32 width{ 0 };
    u32 height{ 0 };
    u8 bitDepth{ 0 };
    u8 colorType{ 0 };
    u8 compressionMethod{ 0 };
    u8 filterMethod{ 0 };
    u8 interlaceMethod{ 0 };
    u8* data{ nullptr };
    u32 dataSize{ 0 };
    u8 pixelSize{ 0 };
};

struct Chunk {
    u32 length{ 0 };
    u32 type{ 0 };
    u8* data{ nullptr };
    u32 crc{ 0 };
};

template <typename T>
inline T endianRead(u8* location) {
    T result;
    for (size_t i = 0; i < sizeof(T); ++i) {
        reinterpret_cast<u8*>(&result)[i] = location[sizeof(T) - i - 1];
    }
    return result;
}

inline b8 load(u8* fileData, size_t fileSize, Png data) {


    const u8 header[8] = { 137, 80, 78, 71, 13, 10, 26, 10 };
    if (::memcmp(fileData, header, 8) != 0) {
        E_PRINT("Invalid PNG header.");
        return false;
    }

    u32 offset{ 8 };


    while (offset < fileSize) {
        Chunk chunk;
        chunk.length = endianRead<u32>(fileData + offset);
        chunk.type = endianRead<u32>(fileData + offset + 4);
        auto ancillary = (chunk.type & 0x20000000) == 0x20000000;
        auto privateBit = (chunk.type & 0x00200000) == 0x00200000;
        auto reservedBit = (chunk.type & 0x00000020) == 0x00000020;
        auto safeToCopy = (chunk.type & 0x00000008) == 0x00000008;
        chunk.data = fileData + offset + 8;
        chunk.crc = (fileData[offset + 8 + chunk.length] << 24) | (fileData[offset + 8 + chunk.length + 1] << 16) | (fileData[offset + 8 + chunk.length + 2] << 8) | fileData[offset + 8 + chunk.length + 3];
        offset += 8 + chunk.length + 4;

        switch(chunk.type) {
            case 0x49484452: { // IHDR
                E_PRINT("IHDR chunk found.");
                data.width = endianRead<u32>(chunk.data + 0);
                data.height = endianRead<u32>(chunk.data + 4);
                data.bitDepth = chunk.data[8];
                data.colorType = chunk.data[9];
                data.compressionMethod = chunk.data[10];
                data.filterMethod = chunk.data[11];
                data.interlaceMethod = chunk.data[12];
            } break;
            case 0x49444154: { // IDAT // well i find this gpt implementation curious let's see what is loaded :) // i feel like i have to rewrite it
                E_PRINT("IDAT chunk found.");

                switch(data.colorType) {
                    case 0: { // grayscale
                        data.pixelSize = 1;
                    } break;
                    case 2: { // RGB
                        data.pixelSize = 3;
                    } break;
                    case 3: { // indexed
                        data.pixelSize = 1;
                    } break;
                    case 4: { // grayscale + alpha
                        data.pixelSize = 2;
                    } break;
                    case 6: { // RGBA
                        data.pixelSize = 4;
                    } break;
                    default: {
                        E_PRINT("Unknown color type.");
                        return false;
                    } break;
                }

                data.dataSize = data.width * data.height * 4;
                data.data = reinterpret_cast<u8*>(std::malloc(data.dataSize)); // TODO: refactor malloc pls
                switch (data.colorType) {
                    case 0: { // grayscale
                        E_PRINT("grayscale");
                        return false;
                    } break;
                    case 2: { // RGB
                        E_PRINT("RGB");
                        for (u32 i = 0, j = 0; j < data.dataSize && i < chunk.length; i += 3, j += 4) {
                            E_ASSERT(i + 2 < chunk.length);
                            E_ASSERT(j + 3 < data.dataSize);
                            data.data[j + 0] = chunk.data[i + 0];
                            data.data[j + 1] = chunk.data[i + 1];
                            data.data[j + 2] = chunk.data[i + 2];
                            data.data[j + 3] = 255;
                        }
                    } break;
                    case 3: { // indexed
                        E_PRINT("indexed");
                        return false;
                    } break;
                    case 4: { // grayscale + alpha
                        E_PRINT("grayscale + alpha");
                        return false;
                    } break;
                    case 6: { // RGBA
                        E_PRINT("RGBA");
                        return false;
                    } break;
                    default: {
                        E_PRINT("Unknown color type.");
                        return false;
                    } break;
                }
            } break;
            case 0x49454E44: { // IEND
                E_PRINT("IEND chunk found.");
            } break;
            case 0x73524742: { // sRGB
                E_PRINT("sRGB chunk found.");
            } break;
            case 0x67414d41: { // gAMA
                E_PRINT("gAMA chunk found.");
            } break;
            case 0x70485973: { // pHYs
                E_PRINT("pHYs chunk found.");
            } break;
            case 0x504C5445: { // PLTE
                E_PRINT("PLTE chunk found.");
            } break;
            case 0x54414449: { // tRNS
                E_PRINT("tRNS chunk found.");
            } break;
            default: {
                E_PRINT("Unknown chunk type. %x", chunk.type);
            } break;
        }
    }
    E_PRINT("PNG loaded successfully.");
    return true;
}

} // namespace base::png