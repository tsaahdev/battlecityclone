#pragma once

#include "common.h"

#include "win32_opengl_defines.h"

#include <cstdio>

namespace base::opengl {



namespace internal {

::HMODULE library{ nullptr };
::HWND helperWindowHandle{ nullptr };
::HDC wglDC{ nullptr };
::HGLRC wglHandle{ nullptr };

struct State {
    b8 ARB_pixelFormat{ false };
    b8 ARB_multisample{ false };
    b8 ARB_framebuffer_srgb{ false };
    b8 EXT_framebuffer_srgb{ false };
    b8 ARB_create_context{ false };
    b8 ARB_create_context_profile{ false };
    b8 ARB_create_context_robustness{ false };
    b8 ARB_context_flush_cntrol{ false };
    b8 ARB_context_no_error{ false };
} state;

inline b8 createHelperWindow(::HINSTANCE instance);
inline b8 initWgl();
inline b8 createContext(::HWND windowHandle);
inline b8 loadOpenGlFunctions();

} // namespace internal

inline b8 init(::HINSTANCE instance, ::HWND windowHandle) {
    using namespace internal;
    // actual wgl -> modern opengl context creation was peeked from GLFW.
    // https://www.glfw.org/

    if (!createHelperWindow(instance)) { return false; }
    if (!initWgl()) { return false; }
    if (!createContext(windowHandle)) { return false; }
    if (!wglMakeCurrent(wglDC, wglHandle)) { return false; }

    // and load rest of the functions
    if (!loadOpenGlFunctions()) { return false; }

    // gotta go rest. gonna finish next time
    ::OutputDebugStringA("\ntest\n"); // TODO: WTH why it doesn't print stuff? am i stoopid? or just tired?

    #if defined(E_DEBUG_BUILD)
        i32 majorVersion{ 0 };
        i32 minorVersion{ 0 };
        glGetIntegerv(GL_MAJOR_VERSION, &majorVersion);
        glGetIntegerv(GL_MINOR_VERSION, &minorVersion);
        c8 buffer[256] = {};
        ::sprintf(buffer, "\nOpenGL version: %d.%d\n", majorVersion, minorVersion);
        ::OutputDebugStringA(buffer);
        ::OutputDebugStringA(glGetString(GL_VERSION));
    #endif

    glClearColor(0.26f, 0.53f, 0.96f, 1.0f); // cornflower blue

    return true;
}

inline void beginFrame() {
    glClear(GL_COLOR_BUFFER_BIT);
}

} // namespace base::opengl

#include "win32_opengl.inl"