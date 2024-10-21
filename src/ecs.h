#pragma once

#include "common.h"

#include <bitset>
#include <vector>

#include "win32_opengl.h"

namespace base::ecs {

struct Entity {
    std::bitset<6> componentMask;
    b8 alive{ false };
};
std::vector<Entity> entities;

struct RenderableSpriteComponent {
    static constexpr u32 componentMask{ 0 };
    v2 position{ 0.0f, 0.0f };
    f32 size{ 1.0f };
    f32 layer{ 0.0f };
    opengl::SpriteId spriteId{ 0 };
};
std::vector<RenderableSpriteComponent> renderableSpriteComponents;
struct MovableComponent {
    static constexpr u32 componentMask{ 1 };
    f32 speed{ 4.0f };
    u8 direction{ 0 };
    b8 moving{ true };
};
std::vector<MovableComponent> movableComponents;
struct InputComponent {
    static constexpr u32 componentMask{ 2 };
    b8 up{ false };
    b8 down{ false };
    b8 left{ false };
    b8 right{ false };
    b8 fire{ false };
    b8 pause{ false };
    b8 start{ false };
};
std::vector<InputComponent> inputComponents;
struct SpriteAnimationComponent {
    static constexpr u32 componentMask{ 3 };
    opengl::SpriteId spriteIdSequence[10]{};

    f32 frameDuration{ 0.0f };
    f32 frameTimer{ 0.0f };
    f32 totalDuration{ 0.0f };
    f32 totalTimer{ 0.0f };
    u8 sequenceLength{ 0 };
    u8 sequenceIndex{ 0 };
    b8 enabled{ false };
};
std::vector<SpriteAnimationComponent> spriteAnimationComponents;

inline u32 createEntity() {
    entities.emplace_back(Entity{});
    auto& entity = entities.back();
    entity.alive = true;

    const auto entityCount = entities.size();
    renderableSpriteComponents.resize(entityCount);
    movableComponents.resize(entityCount);
    inputComponents.resize(entityCount);
    spriteAnimationComponents.resize(entityCount);

    return static_cast<u32>(entities.size() - 1);
}
inline void destroyEntity(u32 entityId) {
    entities[entityId].alive = false;
}
inline void addComponent(u32 entityId, RenderableSpriteComponent&& component) {
    entities[entityId].componentMask.set(RenderableSpriteComponent::componentMask, 1);
    renderableSpriteComponents[entityId] = component;
}
inline void addComponent(u32 entityId, MovableComponent&& component) {
    entities[entityId].componentMask.set(MovableComponent::componentMask, 1);
    movableComponents[entityId] = component;
}
inline InputComponent* addComponent(u32 entityId, InputComponent&& component) {
    entities[entityId].componentMask.set(InputComponent::componentMask, 1);
    inputComponents[entityId] = component;
    return &inputComponents[entityId];
}
inline void addComponent(u32 entityId, SpriteAnimationComponent&& component) {
    entities[entityId].componentMask.set(SpriteAnimationComponent::componentMask, 1);
    spriteAnimationComponents[entityId] = component;
}

inline void inputSystem() {
    u32 entityId = 0;
    for (auto& input: inputComponents) {
        if (!entities[entityId].alive || !entities[entityId].componentMask.test(MovableComponent::componentMask || InputComponent::componentMask)) {
            ++entityId;
            continue;
        }
        auto& movable = movableComponents[entityId];
        movable.moving = true;
        if (input.up) {
            movable.direction = 0;
        } else if (input.down) {
            movable.direction = 2;
        } else if (input.left) {
            movable.direction = 3;
        } else if (input.right) {
            movable.direction = 1;
        } else {
            movable.moving = false;
        }
        ++entityId;
    }
}

inline void moveSystem(f32 dt) {
    u32 entityId = 0;
    for (const auto& movable: movableComponents) {
        if (!entities[entityId].alive || !entities[entityId].componentMask.test(MovableComponent::componentMask || RenderableSpriteComponent::componentMask)) {
            ++entityId;
            continue;
        }
        auto& renderable = renderableSpriteComponents[entityId];
        auto& animation = spriteAnimationComponents[entityId];
        animation.enabled = movable.moving;
        if (movable.direction == 0) {
            renderable.position.x = roundf(renderable.position.x);
            // TODO: add collision detection here
            renderable.position.y += movable.speed * dt * movable.moving;
            animation.spriteIdSequence[0] = opengl::SpriteId::Tank_0_Y_U_0;
            animation.spriteIdSequence[1] = opengl::SpriteId::Tank_0_Y_U_1;
        } else if (movable.direction == 1) {
            // TODO: add collision detection here
            renderable.position.x += movable.speed * dt * movable.moving;
            renderable.position.y = roundf(renderable.position.y);
            animation.spriteIdSequence[0] = opengl::SpriteId::Tank_0_Y_R_0;
            animation.spriteIdSequence[1] = opengl::SpriteId::Tank_0_Y_R_1;
        } else if (movable.direction == 2) {
            renderable.position.x = roundf(renderable.position.x);
            // TODO: add collision detection here
            renderable.position.y -= movable.speed * dt * movable.moving;
            animation.spriteIdSequence[0] = opengl::SpriteId::Tank_0_Y_D_0;
            animation.spriteIdSequence[1] = opengl::SpriteId::Tank_0_Y_D_1;
        } else if (movable.direction == 3) {
            // TODO: add collision detection here
            renderable.position.x -= movable.speed * dt * movable.moving;
            renderable.position.y = roundf(renderable.position.y);
            animation.spriteIdSequence[0] = opengl::SpriteId::Tank_0_Y_L_0;
            animation.spriteIdSequence[1] = opengl::SpriteId::Tank_0_Y_L_1; // TODO: think on how to setup it better.
        }
        ++entityId;
    }
}

inline void animationSystem(f32 dt) {
    u32 entityId = 0;
    for (auto& spriteAnimation: spriteAnimationComponents) {
        if (!entities[entityId].alive || !entities[entityId].componentMask.test(SpriteAnimationComponent::componentMask || RenderableSpriteComponent::componentMask)) {
            ++entityId;
            continue;
        }
        if (!spriteAnimation.enabled) {
            ++entityId;
            continue;
        }
        auto& renderable = renderableSpriteComponents[entityId];
        spriteAnimation.frameTimer += dt;
        spriteAnimation.totalTimer += dt;
        if (spriteAnimation.frameTimer >= spriteAnimation.frameDuration) {
            spriteAnimation.frameTimer = 0.0f;
            ++spriteAnimation.sequenceIndex;
            spriteAnimation.sequenceIndex %= spriteAnimation.sequenceLength;
            renderable.spriteId = spriteAnimation.spriteIdSequence[spriteAnimation.sequenceIndex];
        }
        ++entityId;
    }
}

inline void renderSystem() {
    u32 entityId = 0;
    opengl::pushQuad({}, { 26 }, opengl::SpriteId::None, {});
    for (const auto& sprite: renderableSpriteComponents) { // TODO: use layer
        if (!entities[entityId].alive || !entities[entityId].componentMask.test(RenderableSpriteComponent::componentMask)) {
            ++entityId;
            continue;
        }
        opengl::pushQuad(sprite.position, sprite.size, sprite.spriteId);
        ++entityId;
    }
}


} // namespace base::ecs