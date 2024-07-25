#include <Windows.h>

#include "e_types.h"
#include "win32_opengl.h"

using namespace base;

i32 WINAPI WinMain(::HINSTANCE hInstance, ::HINSTANCE hPrevInstance, c8* lpCmdLine, i32 nCmdShow) {
    static const ::WNDCLASSA wc{
        .style = CS_HREDRAW | CS_VREDRAW,
        .lpfnWndProc = ::DefWindowProcA,
        .hInstance = hInstance,
        .lpszClassName = "BattleCityWindowClass",
    };
    if (::RegisterClassA(&wc) == 0) { return -1; }

    ::HWND hwnd = ::CreateWindowA("BattleCityWindowClass", "Battle City Clone", WS_OVERLAPPEDWINDOW, 0, 0, 800, 600, nullptr, nullptr, hInstance, nullptr);
    if (hwnd == nullptr) { return -1; }


    if (!opengl::load(hInstance, hwnd)) { return -1;}

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