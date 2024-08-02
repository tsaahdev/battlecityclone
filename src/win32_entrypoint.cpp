#include <Windows.h>

#include "common.h"

#include "win32_opengl.h"
#include "win32_opengl.inl"

// https://stackoverflow.com/questions/68469954/how-to-choose-specific-gpu-when-create-opengl-context
extern "C" __declspec(dllexport) ::DWORD NvOptimusEnablement{ 1 };
extern "C" __declspec(dllexport) int AmdPowerXpressRequestHighPerformance{ 1 };

using namespace base;

b8 running{ false };

struct Actions {
    b8 t1_up{ false };
    b8 t1_down{ false };
    b8 t1_left{ false };
    b8 t1_right{ false };
    b8 t1_fire{ false };
    b8 t1_pause{ false };
    b8 t1_start{ false };
} actions;

struct Player {
    opengl::SubTextureId sprite;
    v2 pos;
    i8 dir{ 0 }; // 0 - up, 1 - right, 2 - down, 3 - left
    i8 lives{ 3 };
} player1, player2;

f32 speed = 4.0f;

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
                actions.t1_up = true;
            } else if (wParam == 'S') {
                actions.t1_down = true;
            } else if (wParam == 'A') {
                actions.t1_left = true;
            } else if (wParam == 'D') {
                actions.t1_right = true;
            } else if (wParam == VK_SPACE) {
                actions.t1_fire = true;
            } else if (wParam == VK_RETURN) {
                actions.t1_start = true;
            } else if (wParam == VK_ESCAPE) {
                actions.t1_pause = true;
            }
            return 0;
        }
        case WM_KEYUP: {
            if (wParam == 'W') {
                actions.t1_up = false;
            } else if (wParam == 'S') {
                actions.t1_down = false;
            } else if (wParam == 'A') {
                actions.t1_left = false;
            } else if (wParam == 'D') {
                actions.t1_right = false;
            } else if (wParam == VK_SPACE) {
                actions.t1_fire = false;
            } else if (wParam == VK_RETURN) {
                actions.t1_start = false;
            } else if (wParam == VK_ESCAPE) {
                actions.t1_pause = false;
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

    player1.pos = { -4.0f, -11.0f };
    player1.dir = 0;
    player1.sprite = opengl::SubTextureId::Tank_0_Y_U_0;

    running = true;
    while (running) {
        ::MSG msg{};
        while (::PeekMessageA(&msg, nullptr, 0, 0, PM_REMOVE)) {
            ::TranslateMessage(&msg);
            ::DispatchMessageA(&msg);
        }

        const c8* map = "00000000000000000000000000"
                        "00000000000000000000000000"
                        "00110011001100110011001100"
                        "00110011001100110011001100"
                        "00110011001100110011001100"
                        "00110011001100110011001100"
                        "001100110011cc110011001100"
                        "001100110011cc110011001100"
                        "00110011001100110011001100"
                        "00110011000000000011001100"
                        "00110011000000000011001100"
                        "00000000001100110000000000"
                        "00000000001100110000000000"
                        "11001111000000000011110011"
                        "cc0011110000000000111100cc"
                        "00000000001100110000000000"
                        "00000000001111110000000000"
                        "00110011001111110011001100"
                        "00110011001100110011001100"
                        "00110011001100110011001100"
                        "00110011001100110011001100"
                        "00110011000000000011001100"
                        "00110011000000000011001100"
                        "00110011000111100011001100"
                        "0000000000010b100000000000"
                        "00000000000100100000000000";

        for (i32 y = 0; y < 26; ++y) {
            for (i32 x = 0; x < 26; ++x) {
                const c8 c = map[y * 26 + x];
                if (c == '\0') { break;}
                if (c == '1') {
                    opengl::pushQuad(opengl::SubTextureId::Brick0, { (float)x - 13.0f, 13.0f - (float)y });
                } else if (c == 'c') {
                    opengl::pushQuad(opengl::SubTextureId::Concrete, { (float)x - 13.0f, 13.0f - (float)y });
                } else if (c == 'b') {
                    opengl::pushQuad(opengl::SubTextureId::Bird, { (float)x - 13.0f, 13.0f - (float)y }, 2);
                }
            }
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

        static b8 flip{ false };
        static f32 accmulator{ 0.0f };
        accmulator += dt;
        static const f32 threshold = 2.0f / 60.0f;
        if (accmulator > threshold) {
            accmulator -= threshold;
            flip ^= true;
        }

        if (actions.t1_up) {
            player1.pos.y += speed * dt;
            player1.pos.x = roundf(player1.pos.x);
            player1.dir = 0;
            if (flip) {
                player1.sprite = opengl::SubTextureId::Tank_0_Y_U_0;
            } else {
                player1.sprite = opengl::SubTextureId::Tank_0_Y_U_1;
            }
        } else if (actions.t1_down) {
            player1.pos.y -= speed * dt;
            player1.pos.x = roundf(player1.pos.x);
            player1.dir = 2;
            if (flip) {
                player1.sprite = opengl::SubTextureId::Tank_0_Y_D_1;
            } else {
                player1.sprite = opengl::SubTextureId::Tank_0_Y_D_0;
            }
        } else if (actions.t1_left) {
            player1.pos.x -= speed * dt;
            player1.pos.y = roundf(player1.pos.y);
            player1.dir = 3;
            if (flip) {
                player1.sprite = opengl::SubTextureId::Tank_0_Y_L_0;
            } else {
                player1.sprite = opengl::SubTextureId::Tank_0_Y_L_1;
            }
        } else if (actions.t1_right) {
            player1.pos.x += speed * dt;
            player1.pos.y = roundf(player1.pos.y);
            player1.dir = 1;
            if (flip) {
                player1.sprite = opengl::SubTextureId::Tank_0_Y_R_0;
            } else {
                player1.sprite = opengl::SubTextureId::Tank_0_Y_R_1;
            }

        }

        opengl::pushQuad(player1.sprite, player1.pos, 2);


        opengl::prepareFrame();
        opengl::beginFrame();
        opengl::endFrame();
    }

    return 0;
}