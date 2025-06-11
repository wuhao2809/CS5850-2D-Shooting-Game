#pragma once

#include <unordered_set>
#include <string>
#include "game/ecs/System.hpp"
#include "game/ecs/Entity.hpp"
#include "game/ecs/Component.hpp"
#include "game/ecs/components/Input.hpp"
#include "game/ecs/components/Movement.hpp"
#include "game/ecs/components/Transform.hpp"
#include "game/events/EventManager.hpp"
#include "game/events/EventListener.hpp"
#include "game/events/Event.hpp"
#include "game/events/KeyboardEvent.hpp"
#include "game/ecs/Vector2.hpp"

namespace game {
namespace ecs {
namespace systems {

class EventSystem : public System, public events::EventListener {
private:
    events::EventManager* eventManager;
    std::unordered_set<std::string> pressedKeys;
    Vector2 calculateVelocity(const components::Input* input);

public:
    EventSystem(events::EventManager* eventManager);
    void update(float deltaTime) override;
    void onEvent(const events::Event& event) override;
    void onEntityAdded(const Entity& entity) override;
    void onEntityRemoved(const Entity& entity) override;
};

} // namespace systems
} // namespace ecs
} // namespace game 