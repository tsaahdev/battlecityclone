#pragma once

#if defined(E_DEBUG_BUILD)

#include <stdio.h>

#include "common.h"

namespace base::logger {

inline void print(const c8* message, ...) {
    static const u32 bufferSize{ 4096 };
    c8 buffer[bufferSize] = {};
    ::va_list args;
    va_start(args, message);
    ::vsnprintf(buffer, bufferSize, message, args);
    va_end(args);

    c8 finalBuffer[bufferSize] = {};
    ::sprintf(finalBuffer, "-> %s\n", buffer);

    auto console = ::GetStdHandle(STD_OUTPUT_HANDLE);
    ::WriteConsoleA(console, finalBuffer, static_cast<::DWORD>(::strlen(finalBuffer)), nullptr, nullptr);
}

} // namespace base::logger

#define E_PRINT(...) base::logger::print(__VA_ARGS__)
#else
#define E_PRINT(...)
#endif