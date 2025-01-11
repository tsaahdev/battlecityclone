#pragma once

#include "common.h"
#include "win32_opengl.h"
#include "ecs.h"

namespace base::game {

ecs::Ecs ecs;

E_COMPONENT(Transform) {
    v4 position;
    v2 size;
};

E_COMPONENT(Sprite) {
    opengl::SpriteId spriteId{ 0 };
};

E_COMPONENT(ColoredQuad) {
    v4 color{ 0, 0, 0, 1 };
};

E_COMPONENT(Physics) {
    static inline f32 speed{ 4.0f };
    v2 velocity{ 0 };
    v2 acceleration{ 0 };
};

E_COMPONENT(Input) {
    b8 up{ false };
    b8 down{ false };
    b8 left{ false };
    b8 right{ false };
    b8 fire{ false };
    b8 pause{ false };
    b8 start{ false };
};

E_COMPONENT(Player_1) {
    u8 level{ 0 };
    v2 direction{ 0, 1 };
};
E_COMPONENT(Player_2) {};

inline void createMap0() {
    const c8* mapString =
        "                          "
        "                          "
        "  11  11  11  11  11  11  "
        "  11  11  11  11  11  11  "
        "  11  11  11  11  11  11  "
        "  11  11  11  11  11  11  "
        "  11  11  11cc11  11  11  "
        "  11  11  11cc11  11  11  "
        "  11  11  11  11  11  11  "
        "  11  11          11  11  "
        "  11  11          11  11  "
        "          11  11          "
        "          11  11          "
        "11  1111          1111  11"
        "cc  1111          1111  cc"
        "          11  11          "
        "          111111          "
        "  11  11  111111  11  11  "
        "  11  11  11  11  11  11  "
        "  11  11  11  11  11  11  "
        "  11  11  11  11  11  11  "
        "  11  11          11  11  "
        "  11  11          11  11  "
        "  11  11   1111   11  11  "
        "           1  1           "
        "           1b 1           ";
    const auto backdropId = ecs.createEntity();
    ecs.addComponent<Transform>(backdropId, {
        .position = { 0, 0, -10, 1 },
        .size = { 26 },
    });
    ecs.addComponent<ColoredQuad>(backdropId, {
        .color = {},
    });
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
            const auto entityId = ecs.createEntity();
            ecs.addComponent<Transform>(entityId, {
                .position = { x, 25 - y, 0, 1 },
                .size = size,
            });
            ecs.addComponent<Sprite>(entityId, {
                .spriteId = spriteId
            });
        }
    }
}

// ecs::InputComponent* player1input{ nullptr };

inline void createPlayer1() {
    const auto entityId = ecs.createEntity();
    ecs.addComponent<Player_1>(entityId);
    ecs.addComponent<Transform>(entityId, {
        .position = { 8, 0, 0, 0 },
        .size = { 2, 2 }
    });
    ecs.addComponent<Sprite>(entityId, {
        .spriteId = opengl::SpriteId::Tank_0_Y_U_0
    });
     ecs.addComponent<Physics>(entityId, {
        .velocity = { 0, 0 },
        .acceleration = { 0, 0 }
    });
    // ecs.addComponent(entityId, ecs::MovableComponent{
    //     .speed = 8.0f,
    //     .direction = 0
    // });
    // player1input = ecs::addComponent(entityId, ecs::InputComponent{});
    // ecs.addComponent(entityId, ecs::SpriteAnimationComponent{
    //     .spriteIdSequence = {
    //         opengl::SpriteId::Tank_0_Y_U_0,
    //         opengl::SpriteId::Tank_0_Y_U_1
    //     },
    //     .frameDuration = 2.0f / 60.0f,
    //     .sequenceLength = 2,
    // });
}

// inline void updatePlayer1Input() {
//     player1input->up = (::GetAsyncKeyState('W') & 0x8000) == 0x8000;
//     player1input->down =( ::GetAsyncKeyState('S') & 0x8000) == 0x8000;
//     player1input->left = (::GetAsyncKeyState('A') & 0x8000) == 0x8000;
//     player1input->right = (::GetAsyncKeyState('D') & 0x8000) == 0x8000;
//     player1input->fire = (::GetAsyncKeyState(VK_SPACE) & 0x8000) == 0x8000;
//     player1input->pause = (::GetAsyncKeyState(VK_ESCAPE) & 0x8000) == 0x8000;
//     player1input->start = (::GetAsyncKeyState(VK_RETURN) & 0x8000) == 0x8000;
// }
void init() {
    createMap0();
    createPlayer1();
}

void update(f32 dt) {
    b8 b_up = (::GetAsyncKeyState('W') & 0x8000) == 0x8000;
    b8 b_down =( ::GetAsyncKeyState('S') & 0x8000) == 0x8000;
    b8 b_left = (::GetAsyncKeyState('A') & 0x8000) == 0x8000;
    b8 b_right = (::GetAsyncKeyState('D') & 0x8000) == 0x8000;
    b8 b_fire = (::GetAsyncKeyState(VK_SPACE) & 0x8000) == 0x8000;
    b8 b_pause = (::GetAsyncKeyState(VK_ESCAPE) & 0x8000) == 0x8000;
    b8 b_start = (::GetAsyncKeyState(VK_RETURN) & 0x8000) == 0x8000;


    ecs::System<Player_1, Transform, Physics>::update(ecs, [&](Player_1&, Transform& transform, Physics& physics){
        const auto s = Physics::speed * dt;
        physics.velocity = {};

        if (b_left) { physics.velocity.x = -s; }
        else if (b_right) { physics.velocity.x = s; }
        else if (b_up) { physics.velocity.y = s; }
        else if (b_down) { physics.velocity.y = -s; }
    });

    ecs::System<Transform, Physics>::update(ecs, [&](Transform& transform, Physics& physics){
        transform.position.x += physics.velocity.x;
        transform.position.y += physics.velocity.y;

        if (b_left || b_right) { transform.position.y = std::roundf(transform.position.y); }
        else if (b_up || b_down) { transform.position.x = std::roundf(transform.position.x); }
    });

    ecs::System<Transform, ColoredQuad>::update(ecs, [&](Transform& transform, ColoredQuad& coloredQuad){
        opengl::pushQuad(transform.position, transform.size, opengl::SpriteId::None, coloredQuad.color);
    });

    ecs::System<Transform, Sprite>::update(ecs, [&](Transform& transform, Sprite& sprite){
        opengl::pushQuad(transform.position, transform.size, sprite.spriteId);
    });
}

} // namespace base::game
