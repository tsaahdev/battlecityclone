#include <Windows.h>

#include "common.h"

#include "win32_opengl.h"
#include "win32_opengl.inl"
#include "e_png.h"

// https://stackoverflow.com/questions/68469954/how-to-choose-specific-gpu-when-create-opengl-context
extern "C" __declspec(dllexport) ::DWORD NvOptimusEnablement{ 1 };
extern "C" __declspec(dllexport) int AmdPowerXpressRequestHighPerformance{ 1 };

using namespace base;

b8 running{ false };

::LRESULT CALLBACK wndProc(::HWND hwnd, ::UINT msg, ::WPARAM wParam, ::LPARAM lParam) {
    switch (msg) {
        case WM_CLOSE: {
            running = false;
            ::PostQuitMessage(0);
            return 0;
        }
        case WM_SIZE: {
            const i32 width = LOWORD(lParam);
            const i32 height = HIWORD(lParam);
            opengl::resize(width, height);
            return 0;
        }
    }
    return ::DefWindowProcA(hwnd, msg, wParam, lParam);
}

#if defined(E_DEBUG_BUILD)
i32 main() {
    const auto hInstance = ::GetModuleHandleA(nullptr);
#else
i32 WINAPI WinMain(::HINSTANCE hInstance, ::HINSTANCE hPrevInstance, c8* lpCmdLine, i32 nCmdShow) {
#endif
    static const ::WNDCLASSEXA wc{
        .cbSize = sizeof(::WNDCLASSEXA),
        .style = CS_HREDRAW | CS_VREDRAW,
        .lpfnWndProc = wndProc,
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

    if (!opengl::init(hInstance, hwnd, 800, 600)) { return -1; }

    {
        
        // u32 fileSize{ 0 };
        // const u32 bufferSize{ 4096 };
        // u8 buffer[bufferSize];
        // filesystem::readEntireFile("../assets/test_opaque.png", buffer, bufferSize, fileSize);
        // png::Png png;
        // png::load(buffer, fileSize, png);
        
        opengl::wip_test_init();

    }

    ::ShowWindow(hwnd, SW_SHOW);


    running = true;
    while (running) {
        ::MSG msg{};
        while (::PeekMessageA(&msg, nullptr, 0, 0, PM_REMOVE)) {
            ::TranslateMessage(&msg);
            ::DispatchMessageA(&msg);
        }
        opengl::beginFrame();
        opengl::endFrame();
    }

    return 0;
}