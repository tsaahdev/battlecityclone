#pragma once

#include "common.h"

#include <bitset>
#include <vector>
#include <array>
#include <string>
#include <unordered_map>
#include <numeric>
#include <typeindex>
#include <type_traits>

#include "win32_opengl.h"
#include "e_sparse_array.h"

namespace base::ecs {

using EntityId = u16;
constexpr EntityId INVALID_ENTITY_ID{ std::numeric_limits<EntityId>::max() };

class Ecs {
public:
    void clear() {
        availableEntities_.clear();
        entityMasks_.clear();
        entityNames_.clear();
        for (auto& data: componentDataArray_) {
            data->clear();
        }
        maxEntityId_ = 0;
    }

    EntityId createEntity(std::string&& name = {}) {
        auto id = INVALID_ENTITY_ID;
        if (availableEntities_.empty()) {
            id = maxEntityId_++;
        } else {
            id = availableEntities_.back();
            availableEntities_.pop_back();
        }
        entityMasks_.set(id, {});
        if (!name.empty()) {
            entityNames_.emplace(id, std::move(name));
        }
        return id;
    }

    void removeEntity(EntityId id) {
        for (auto& c: componentDataArray_) {
            c->remove(id);
        }
        entityMasks_.remove(id);
        entityNames_.erase(id);
        availableEntities_.push_back(id);
    }

    std::string entityName(EntityId id) const {
        if (entityNames_.contains(id)) {
            return entityNames_.at(id);
        }
        return "Entity_" + std::to_string(id);
    }

    template <class ComponentType>
    ComponentType& addComponent(EntityId id, ComponentType&& component = {}) {
        auto& data = ComponentType::data();
        componentDataArray_[ComponentType::id()] = &data;
        if (data.get(id)) {
            return data.set(id, std::move(component));
        }
        auto& mask = entityMask(id);
        setComponentBit<ComponentType>(mask, true);
        return data.set(id, std::move(component));
    }

    template <class ComponentType>
    ComponentType& getComponentRef(EntityId id) {
        auto& data = ComponentType::data();
        return data.getRef(id);
    }

    template <class ComponentType>
    ComponentType* getComponent(EntityId id) {
        auto& data = ComponentType::data();
        return data.get(id);
    }

    template <class ComponentType>
    void removeComponent(EntityId id) {
        auto& data = ComponentType::data();
        if (data.get(id) == nullptr) { return; }
        auto& mask = entityMask(id);
        setComponentBit<ComponentType>(mask, false);
        data.remove(id);
    }

    template <class... ComponentTypes>
    b8 hasAllComponents(EntityId id) {
        return (hasComponent<ComponentTypes>(id) && ...);
    }

    template <class... ComponentTypes>
    b8 hasAnyComponents(EntityId id) {
        return (hasComponent<ComponentTypes>(id) || ...);
    }

    template <class ComponentType>
    b8 hasComponent(EntityId id) {
        return componentBit<ComponentType>(entityMask(id));
    }

private:
    static inline constexpr size_t MAX_COMPONENTS{ 64 };
    using ComponentMask = std::bitset<MAX_COMPONENTS>;
    EntityId maxEntityId_{ 0 };
    std::vector<EntityId> availableEntities_;
    SparseArray<EntityId, ComponentMask> entityMasks_;
    std::unordered_map<EntityId, std::string> entityNames_;
    std::array<BaseSparseArray<EntityId>*, MAX_COMPONENTS> componentDataArray_;

    ComponentMask& entityMask(EntityId id) {
        return entityMasks_.getRef(id);
    }

    template <class ComponentType>
    void setComponentBit(ComponentMask& mask, b8 val) {
        mask.set(ComponentType::id(), val);
    }

    template <class ComponentType>
    b8 componentBit(ComponentMask& mask) {
        return mask.test(ComponentType::id());
    }
};

struct ComponentCounter {
    static size_t generateId() {
        static size_t id{ 0 };
        return id++;
    }
    static size_t generateMask() {
        static size_t count{ 0 };
        return 1ui64 << count++;
    }
};

template <class...Components>
class System {
public:
    template <class Kernel>
    static void update(Ecs& ecs, Kernel&& kernel) {
        idListPointerArray_ = { &Components::realIndexList()... };
        componentStoragePointerArray_ = { &Components::data()... };
        auto indexSequence = std::make_index_sequence<sizeof...(Components)>{};

        const auto& smallestIdList = findSmallestIdList();
        for (const auto& id: *smallestIdList) {
            if (ecs.hasAllComponents<Components...>(id)) {
                if constexpr (std::is_invocable_v<Kernel, EntityId, Components&...>) {
                    std::apply(kernel, std::tuple_cat(std::make_tuple(id), makeComponentTuple(id, indexSequence)));
                } else if constexpr (std::is_invocable_v<Kernel, Components&...>) {
                    std::apply(kernel, makeComponentTuple(id, indexSequence));
                } else {
                    E_ASSERT(false);
                }
            }
        }
    }
private:
    template <size_t index>
    using GetComponentType = std::tuple_element_t<index, std::tuple<Components...>>;

    static inline std::array<BaseSparseArray<EntityId>*, sizeof...(Components)> componentStoragePointerArray_;
    static inline std::array<std::vector<EntityId>*, sizeof...(Components)> idListPointerArray_;

    template <size_t index>
    static constexpr auto& getComponentStorageAt() {
        using ComponentType = typename System::template GetComponentType<index>;
        return ComponentType::data();
    }

    template <size_t...indice>
    static constexpr auto makeComponentTuple(EntityId id, const std::index_sequence<indice...>&) {
        return std::make_tuple((std::ref(getComponentStorageAt<indice>().getRef(id)))...);
    }

    static std::vector<EntityId>* findSmallestIdList() {
        if constexpr (sizeof...(Components) == 1) {
            return idListPointerArray_.front();
        } else {
            std::vector<EntityId>* smallestIdList = nullptr;
            auto smallestSize = std::numeric_limits<size_t>::max();
            for (const auto& s: idListPointerArray_) {
                const auto size = s->size();
                if (size < smallestSize) {
                    smallestSize = size;
                    smallestIdList = s;
                }
            }
            return smallestIdList;
        }
    }
};

template <class T>
struct Component {
public:
    static constexpr size_t id() { return id_; }
    static constexpr size_t mask() { return mask_; }
    static constexpr SparseArray<EntityId, T>& data() { return data_; }
    static constexpr std::vector<EntityId>& realIndexList() { return data_.realIndexList(); }
    static constexpr size_t size() { return data_.size(); }
    static constexpr void remove(EntityId eid) { data_.remove(eid); }
private:
    static inline SparseArray<EntityId, T> data_;
    static inline const size_t id_{ ComponentCounter::generateId() };
    static inline const size_t mask_{ ComponentCounter::generateMask() };
};
#define E_COMPONENT(name) struct name: public base::ecs::Component<name>









// struct RenderableSpriteComponent {
//     static constexpr u32 componentMask{ 0 };
//     v2 position{ 0.0f, 0.0f };
//     f32 size{ 1.0f };
//     f32 layer{ 0.0f };
//     opengl::SpriteId spriteId{ 0 };
// };
// std::vector<RenderableSpriteComponent> renderableSpriteComponents;
// struct MovableComponent {
//     static constexpr u32 componentMask{ 1 };
//     f32 speed{ 4.0f };
//     u8 direction{ 0 };
//     b8 moving{ true };
// };
// std::vector<MovableComponent> movableComponents;
// struct InputComponent {
//     static constexpr u32 componentMask{ 2 };
//     b8 up{ false };
//     b8 down{ false };
//     b8 left{ false };
//     b8 right{ false };
//     b8 fire{ false };
//     b8 pause{ false };
//     b8 start{ false };
// };
// std::vector<InputComponent> inputComponents;
// struct SpriteAnimationComponent {
//     static constexpr u32 componentMask{ 3 };
//     opengl::SpriteId spriteIdSequence[10]{};

//     f32 frameDuration{ 0.0f };
//     f32 frameTimer{ 0.0f };
//     f32 totalDuration{ 0.0f };
//     f32 totalTimer{ 0.0f };
//     u8 sequenceLength{ 0 };
//     u8 sequenceIndex{ 0 };
//     b8 enabled{ false };
// };
// std::vector<SpriteAnimationComponent> spriteAnimationComponents;

// inline u32 createEntity() {
//     entities.emplace_back(Entity{});
//     auto& entity = entities.back();
//     entity.alive = true;

//     const auto entityCount = entities.size();
//     renderableSpriteComponents.resize(entityCount);
//     movableComponents.resize(entityCount);
//     inputComponents.resize(entityCount);
//     spriteAnimationComponents.resize(entityCount);

//     return static_cast<u32>(entities.size() - 1);
// }
// inline void destroyEntity(u32 entityId) {
//     entities[entityId].alive = false;
// }
// inline void addComponent(u32 entityId, RenderableSpriteComponent&& component) {
//     entities[entityId].componentMask.set(RenderableSpriteComponent::componentMask, 1);
//     renderableSpriteComponents[entityId] = component;
// }
// inline void addComponent(u32 entityId, MovableComponent&& component) {
//     entities[entityId].componentMask.set(MovableComponent::componentMask, 1);
//     movableComponents[entityId] = component;
// }
// inline InputComponent* addComponent(u32 entityId, InputComponent&& component) {
//     entities[entityId].componentMask.set(InputComponent::componentMask, 1);
//     inputComponents[entityId] = component;
//     return &inputComponents[entityId];
// }
// inline void addComponent(u32 entityId, SpriteAnimationComponent&& component) {
//     entities[entityId].componentMask.set(SpriteAnimationComponent::componentMask, 1);
//     spriteAnimationComponents[entityId] = component;
// }

// inline void inputSystem() {
//     u32 entityId = 0;
//     for (auto& input: inputComponents) {
//         if (!entities[entityId].alive || !entities[entityId].componentMask.test(MovableComponent::componentMask || InputComponent::componentMask)) {
//             ++entityId;
//             continue;
//         }
//         auto& movable = movableComponents[entityId];
//         movable.moving = true;
//         if (input.up) {
//             movable.direction = 0;
//         } else if (input.down) {
//             movable.direction = 2;
//         } else if (input.left) {
//             movable.direction = 3;
//         } else if (input.right) {
//             movable.direction = 1;
//         } else {
//             movable.moving = false;
//         }
//         ++entityId;
//     }
// }

// inline void moveSystem(f32 dt) {
//     u32 entityId = 0;
//     for (const auto& movable: movableComponents) {
//         if (!entities[entityId].alive || !entities[entityId].componentMask.test(MovableComponent::componentMask || RenderableSpriteComponent::componentMask)) {
//             ++entityId;
//             continue;
//         }
//         auto& renderable = renderableSpriteComponents[entityId];
//         auto& animation = spriteAnimationComponents[entityId];
//         animation.enabled = movable.moving;
//         if (movable.direction == 0) {
//             renderable.position.x = roundf(renderable.position.x);
//             // TODO: add collision detection here
//             renderable.position.y += movable.speed * dt * movable.moving;
//             animation.spriteIdSequence[0] = opengl::SpriteId::Tank_0_Y_U_0;
//             animation.spriteIdSequence[1] = opengl::SpriteId::Tank_0_Y_U_1;
//         } else if (movable.direction == 1) {
//             // TODO: add collision detection here
//             renderable.position.x += movable.speed * dt * movable.moving;
//             renderable.position.y = roundf(renderable.position.y);
//             animation.spriteIdSequence[0] = opengl::SpriteId::Tank_0_Y_R_0;
//             animation.spriteIdSequence[1] = opengl::SpriteId::Tank_0_Y_R_1;
//         } else if (movable.direction == 2) {
//             renderable.position.x = roundf(renderable.position.x);
//             // TODO: add collision detection here
//             renderable.position.y -= movable.speed * dt * movable.moving;
//             animation.spriteIdSequence[0] = opengl::SpriteId::Tank_0_Y_D_0;
//             animation.spriteIdSequence[1] = opengl::SpriteId::Tank_0_Y_D_1;
//         } else if (movable.direction == 3) {
//             // TODO: add collision detection here
//             renderable.position.x -= movable.speed * dt * movable.moving;
//             renderable.position.y = roundf(renderable.position.y);
//             animation.spriteIdSequence[0] = opengl::SpriteId::Tank_0_Y_L_0;
//             animation.spriteIdSequence[1] = opengl::SpriteId::Tank_0_Y_L_1; // TODO: think on how to setup it better.
//         }
//         ++entityId;
//     }
// }

// inline void animationSystem(f32 dt) {
//     u32 entityId = 0;
//     for (auto& spriteAnimation: spriteAnimationComponents) {
//         if (!entities[entityId].alive || !entities[entityId].componentMask.test(SpriteAnimationComponent::componentMask || RenderableSpriteComponent::componentMask)) {
//             ++entityId;
//             continue;
//         }
//         if (!spriteAnimation.enabled) {
//             ++entityId;
//             continue;
//         }
//         auto& renderable = renderableSpriteComponents[entityId];
//         spriteAnimation.frameTimer += dt;
//         spriteAnimation.totalTimer += dt;
//         if (spriteAnimation.frameTimer >= spriteAnimation.frameDuration) {
//             spriteAnimation.frameTimer = 0.0f;
//             ++spriteAnimation.sequenceIndex;
//             spriteAnimation.sequenceIndex %= spriteAnimation.sequenceLength;
//             renderable.spriteId = spriteAnimation.spriteIdSequence[spriteAnimation.sequenceIndex];
//         }
//         ++entityId;
//     }
// }

// inline void renderSystem() {
//     u32 entityId = 0;
//     opengl::pushQuad({}, { 26 }, opengl::SpriteId::None, {});
//     for (const auto& sprite: renderableSpriteComponents) { // TODO: use layer
//         if (!entities[entityId].alive || !entities[entityId].componentMask.test(RenderableSpriteComponent::componentMask)) {
//             ++entityId;
//             continue;
//         }
//         opengl::pushQuad(sprite.position, sprite.size, sprite.spriteId);
//         ++entityId;
//     }
// }


} // namespace base::ecs

