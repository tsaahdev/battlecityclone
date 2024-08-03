#include <Windows.h>

#include "common.h"

#include "win32_opengl.h"
#include "win32_opengl.inl"

#include "map.h"

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
        case WM_KEYDOWN: {
            if (wParam == 'W') {
                map::actions.t1_up = true;
            } else if (wParam == 'S') {
                map::actions.t1_down = true;
            } else if (wParam == 'A') {
                map::actions.t1_left = true;
            } else if (wParam == 'D') {
                map::actions.t1_right = true;
            } else if (wParam == VK_SPACE) {
                map::actions.t1_fire = true;
            } else if (wParam == VK_RETURN) {
                map::actions.t1_start = true;
            } else if (wParam == VK_ESCAPE) {
                map::actions.t1_pause = true;
            }
            return 0;
        }
        case WM_KEYUP: {
            if (wParam == 'W') {
                map::actions.t1_up = false;
            } else if (wParam == 'S') {
                map::actions.t1_down = false;
            } else if (wParam == 'A') {
                map::actions.t1_left = false;
            } else if (wParam == 'D') {
                map::actions.t1_right = false;
            } else if (wParam == VK_SPACE) {
                map::actions.t1_fire = false;
            } else if (wParam == VK_RETURN) {
                map::actions.t1_start = false;
            } else if (wParam == VK_ESCAPE) {
                map::actions.t1_pause = false;
            }
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

        opengl::wip_test_init();
        opengl::resize(800, 600);
    }

    ::ShowWindow(hwnd, SW_SHOW);

    i64 perfFreq{ 0 };
    ::QueryPerformanceFrequency(reinterpret_cast<::LARGE_INTEGER*>(&perfFreq));
    i64 lastCounter{ 0 };
    ::QueryPerformanceCounter(reinterpret_cast<::LARGE_INTEGER*>(&lastCounter));

    map::Map map; // ugly
    map::createMap0(map);
    map::placeTank(map, 8, 24, map::Side::Player1);

    running = true;
    while (running) {
        ::MSG msg{};
        while (::PeekMessageA(&msg, nullptr, 0, 0, PM_REMOVE)) {
            ::TranslateMessage(&msg);
            ::DispatchMessageA(&msg);
        }

        

        // TODO: make a map thing better
        // I guess i want a separate thing for a map
        // i want to be able to query it for tile ocupation
        // i want to store tile information in it
        // i want to keep it's related draw data in a gl buffer and modify it only on change
        // (i guess whould be faster to rewrite whole map than to update several individual tiles that changed) // TODO: benchmark it?
        // a tank will occupy 4 tiles
        // a bullet will occupy 2 tiles or don't even store a bullet there, just query the map for it's position
        // mayb i store a 
        // - v2i tile position
        // - v2 offset
        // - f32 tile scale
        // - u32 sprite id
        // - f32 layer

        // may b it should be like a map of tiles just for map  and similar thing for collision only where I can refer to a map?
        // but for faster queryes and modifies no clue but seems to me that it can be a separate thing

        i64 now{ 0 };
        ::QueryPerformanceCounter(reinterpret_cast<::LARGE_INTEGER*>(&now));
        i64 nanoseconds = ((now - lastCounter) * 1'000'000'000) / perfFreq;
        f32 dt = (now - lastCounter) / static_cast<f32>(perfFreq);
        lastCounter = now;

        map::update(map, dt);

        map::pushQuads(map);

        opengl::prepareFrame();
        opengl::beginFrame();
        opengl::endFrame();
    }

    return 0;
}