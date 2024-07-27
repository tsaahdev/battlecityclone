#include <Windows.h>

#include "common.h"

#include "win32_opengl.h"

// https://stackoverflow.com/questions/68469954/how-to-choose-specific-gpu-when-create-opengl-context
extern "C" __declspec(dllexport) ::DWORD NvOptimusEnablement{ 1 };
extern "C" __declspec(dllexport) int AmdPowerXpressRequestHighPerformance{ 1 };

using namespace base;

#if defined(E_DEBUG_BUILD)
i32 main() {
    const auto hInstance = ::GetModuleHandleA(nullptr);
#else
i32 WINAPI WinMain(::HINSTANCE hInstance, ::HINSTANCE hPrevInstance, c8* lpCmdLine, i32 nCmdShow) {
#endif
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

    opengl::wip_test_init();

    ::ShowWindow(hwnd, SW_SHOW);

    while (true) {
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