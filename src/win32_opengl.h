#pragma once

#include <Windows.h>

#include "e_types.h"

namespace base::opengl {

using FPTR_wglCreateContext = ::HGLRC(__stdcall*)(::HDC);

FPTR_wglCreateContext wglCreateContext{ nullptr };

::HMODULE library{ nullptr };
::HWND helperWindowHandle{ nullptr };

b8 load(::HINSTANCE instance, ::HWND windowHandle) {
    library = ::LoadLibraryA("opengl32.dll");
    if (!library) { return false; }

    wglCreateContext = reinterpret_cast<FPTR_wglCreateContext>(::GetProcAddress(library, "wglCreateContext"));
    if (!wglCreateContext) { return false; }

    static const ::WNDCLASSA wc{
        .style = CS_OWNDC,
        .lpfnWndProc = ::DefWindowProcA,
        .hInstance = ::GetModuleHandleA(nullptr),
        .lpszClassName = "BattleCityOpenglHelperWindowClass",
    };
    if (::RegisterClassA(&wc) == 0) { return false; }

    helperWindowHandle = ::CreateWindowA("BattleCityOpenglHelperWindowClass", "Battle City Opengl Helper Window", WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 0, 0, 1, 1, nullptr, nullptr, instance, nullptr);
    if (!helperWindowHandle) { return false; }

    ::ShowWindow(helperWindowHandle, SW_HIDE);

    ::MSG msg;
    while (::PeekMessageA(&msg, helperWindowHandle, 0, 0, PM_REMOVE)) {
        ::TranslateMessage(&msg);
        ::DispatchMessageA(&msg);
    }

    const auto dicrectContext = ::GetDC(helperWindowHandle);
    if (!dicrectContext) { return false; }

    static const ::PIXELFORMATDESCRIPTOR pixelFormatDescriptor{
        .nSize = sizeof(::PIXELFORMATDESCRIPTOR),
        .nVersion = 1,
        .dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
        .iPixelType = PFD_TYPE_RGBA,
        .cColorBits = 24,
    };

    // find pixel format that suits us and set it
    // get dummy wgl context
    // make it current
    // get context and dc
    // load extensions
    // get capabilities
    // make current dispose of dummy context

    // create real context
    // and load rest of the functions


    return true;
}




} // namespace base::opengl