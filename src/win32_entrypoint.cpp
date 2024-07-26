#include <Windows.h>

#include "common.h"

#include "win32_opengl.h"

using namespace base;

i32 WINAPI WinMain(::HINSTANCE hInstance, ::HINSTANCE hPrevInstance, c8* lpCmdLine, i32 nCmdShow) {
    static const ::WNDCLASSEXA wc{
        .cbSize = sizeof(::WNDCLASSEXA),
        .style = CS_HREDRAW | CS_VREDRAW,
        .lpfnWndProc = ::DefWindowProcA,
        .hInstance = hInstance,
        .lpszClassName = "BattleCityWindowClass",
    };
    if (::RegisterClassExA(&wc) == 0) { return -1; }

    ::HWND hwnd = ::CreateWindowExA(
        0,
        "BattleCityWindowClass",
        "Battle City Clone",
        WS_OVERLAPPEDWINDOW,
        0, 0, 800, 600,
        nullptr, nullptr, hInstance, nullptr
    );
    if (hwnd == nullptr) { return -1; }


    if (!opengl::init(hInstance, hwnd)) { return -1; }

    ::ShowWindow(hwnd, nCmdShow);

    while (true) {
        ::MSG msg{};
        while (::PeekMessageA(&msg, nullptr, 0, 0, PM_REMOVE)) {
            ::TranslateMessage(&msg);
            ::DispatchMessageA(&msg);
        }
    }

    return 0;
}