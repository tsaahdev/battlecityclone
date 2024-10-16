#pragma once

#include "common.h"
#include "win32_opengl.h"
#include "ecs.h"

namespace base::game {

inline void createMap0() {
    const c8* mapString =
        "00000000000000000000000000"
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
        "00000000000100100000000000"
        "000000000001b0100000000000";
    for (i32 y = 0; y < 26; ++y) {
        for (i32 x = 0; x < 26; ++x) {
            opengl::SpriteId spriteId{ opengl::SpriteId::Snow };
            f32 size = 1.0f;
            const c8 c = mapString[y * 26 + x];
            if (c == '\0') { break;}
            if (c == '1') {
                spriteId = opengl::SpriteId::Brick0;
            } else if (c == 'c') {
                spriteId = opengl::SpriteId::Concrete;
            } else if (c == 'b') {
                spriteId = opengl::SpriteId::Bird;
                size = 2.0f;
            } else {
                continue;
            }
            const auto entityId = ecs::createEntity();
            ecs::addComponent(entityId, ecs::RenderableSpriteComponent{
                .position = { (f32)x, 26.0f - (f32)y },
                .size = size,
                .layer = 0.0f,
                .spriteId = spriteId
            });
        }
    }
}

ecs::InputComponent* player1input{ nullptr };
inline void createPlayer1() {
    const auto entityId = ecs::createEntity();
    ecs::addComponent(entityId, ecs::RenderableSpriteComponent{
        .position = { 8.0f, 1.0f },
        .size = 2.0f,
        .layer = 0.0f,
        .spriteId = opengl::SpriteId::Tank_0_Y_U_0
    });
    ecs::addComponent(entityId, ecs::MovableComponent{
        .speed = 8.0f,
        .direction = 0
    });
    player1input = ecs::addComponent(entityId, ecs::InputComponent{});
    ecs::addComponent(entityId, ecs::SpriteAnimationComponent{
        .spriteIdSequence = {
            opengl::SpriteId::Tank_0_Y_U_0,
            opengl::SpriteId::Tank_0_Y_U_1
        },
        .frameDuration = 2.0f / 60.0f,
        .sequenceLength = 2,
    });
}

inline void updatePlayer1Input() {
    player1input->up = (::GetAsyncKeyState('W') & 0x8000) == 0x8000;
    player1input->down =( ::GetAsyncKeyState('S') & 0x8000) == 0x8000;
    player1input->left = (::GetAsyncKeyState('A') & 0x8000) == 0x8000;
    player1input->right = (::GetAsyncKeyState('D') & 0x8000) == 0x8000;
    player1input->fire = (::GetAsyncKeyState(VK_SPACE) & 0x8000) == 0x8000;
    player1input->pause = (::GetAsyncKeyState(VK_ESCAPE) & 0x8000) == 0x8000;
    player1input->start = (::GetAsyncKeyState(VK_RETURN) & 0x8000) == 0x8000;
}





} // namespace base::game