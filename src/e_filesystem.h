#pragma once

#include <Windows.h>

#include "common.h"

namespace base::filesystem {

inline b8 readEntireFile(const c8* path, u8* outBuffer, const u32& outBufferSize, u32& outFileSize) {
    auto fileHandle = ::CreateFileA(path, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (fileHandle == INVALID_HANDLE_VALUE) { E_PRINT("Failed to open file '%s'", path); return false; }
    size_t fileSize{ 0 };
    ::GetFileSizeEx(fileHandle, reinterpret_cast<::LARGE_INTEGER*>(&fileSize));
    outFileSize = static_cast<u32>(fileSize);
    if (outFileSize == 0) { E_PRINT("File '%s' is empty", path); return false; }
    if (outFileSize > outBufferSize) { E_PRINT("File '%s' is too large", path); return false; }
    ::DWORD bytesRead{ 0 };
    ::ReadFile(fileHandle, outBuffer, outBufferSize, &bytesRead, nullptr);
    ::CloseHandle(fileHandle);
    if (bytesRead != outFileSize) { E_PRINT("Failed to read entire file '%s'", path); return false; }

    return true;
}

} // namespace base::filesystem