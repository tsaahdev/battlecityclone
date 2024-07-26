#pragma once

#include "win32_opengl_defines.h"

namespace base::opengl {



namespace internal {

::HMODULE library{ nullptr };
::HWND helperWindowHandle{ nullptr };
::HDC windowDc{ nullptr };

struct State {
    b8 ARB_pixelFormat{ false };
    b8 ARB_multisample{ false };
    b8 ARB_framebuffer_srgb{ false };
    b8 EXT_framebuffer_srgb{ false };
    
} state;

inline b8 createHelperWindow(::HINSTANCE instance);
inline b8 initWgl();
inline b8 createContext(::HWND windowHandle);

} // namespace internal

b8 init(::HINSTANCE instance, ::HWND windowHandle) {
    using namespace internal;
    // actual wgl -> modern opengl context creation was peeked from GLFW.
    // https://www.glfw.org/

    if (!createHelperWindow(instance)) { return false; }
    if (!initWgl()) { return false; }
    if (!createContext(windowHandle)) { return false; }
    // and load rest of the functions
    return true;
}

} // namespace base::opengl

#include "win32_opengl.inl"