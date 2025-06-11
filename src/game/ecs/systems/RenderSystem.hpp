#pragma once

#include "../System.hpp"
#include "../ComponentManager.hpp"
#include "../Entity.hpp"
#include "../components/Transform.hpp"
#include "../components/Sprite.hpp"
#include <SDL3/SDL.h>

namespace game {
    class GameWorld;  // Forward declaration
}

namespace game::ecs::systems {

class RenderSystem : public System {
public:
    RenderSystem();
    void update(float deltaTime) override;
    void onEntityAdded(const Entity& entity) override;
    void onEntityRemoved(const Entity& entity) override;
};

} // namespace game::ecs::systems 