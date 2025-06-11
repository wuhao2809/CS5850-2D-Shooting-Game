#include "game/ecs/systems/EventSystem.hpp"
#include "game/ecs/ComponentManager.hpp"
#include "game/ecs/components/Input.hpp"
#include "game/ecs/components/Movement.hpp"
#include "game/ecs/components/Transform.hpp"
#include "game/events/EventManager.hpp"
#include "game/events/KeyboardEvent.hpp"
#include <SDL3/SDL.h>
#include <algorithm>
#include <iostream>

namespace game {
namespace ecs {
namespace systems {

EventSystem::EventSystem(events::EventManager* eventManager) 
    : eventManager(eventManager) {
    SDL_LogInfo(SDL_LOG_CATEGORY_INPUT, "[EventSystem] Initializing with EventManager: %p", eventManager);
    
    if (!eventManager) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "[EventSystem] EventManager is null!");
        return;
    }
    
    // Register required components
    registerComponent<components::Transform>();
    registerComponent<components::Movement>();
    registerComponent<components::Input>();
    SDL_LogInfo(SDL_LOG_CATEGORY_INPUT, "[EventSystem] Components registered");

    // Subscribe to keyboard events
    SDL_LogInfo(SDL_LOG_CATEGORY_INPUT, "[EventSystem] Subscribing to keyboard events");
    eventManager->subscribe("keyboard", this);
    SDL_LogInfo(SDL_LOG_CATEGORY_INPUT, "[EventSystem] Subscription complete");
}

void EventSystem::update(float deltaTime) {
    SDL_LogInfo(SDL_LOG_CATEGORY_INPUT, "[EventSystem] Update called with %zu entities", getEntities().size());
    
    for (const auto& entity : getEntities()) {
        auto* input = ComponentManager::getInstance().getComponent<components::Input>(entity);
        auto* movement = ComponentManager::getInstance().getComponent<components::Movement>(entity);
        
        if (input && movement && input->isEnabled()) {
            Vector2 newVelocity = calculateVelocity(input);
            if (newVelocity != movement->getVelocity()) {
                SDL_LogInfo(SDL_LOG_CATEGORY_INPUT, "[EventSystem] Updating velocity for entity %llu: (%.2f, %.2f)", 
                    entity.getId(), newVelocity.x, newVelocity.y);
                movement->setVelocity(newVelocity);
            }
        }
    }
}

void EventSystem::onEvent(const events::Event& event) {
    SDL_LogInfo(SDL_LOG_CATEGORY_INPUT, "[EventSystem] Received event of type: %s", typeid(event).name());
    
    if (auto* keyboardEvent = dynamic_cast<const events::KeyboardEvent*>(&event)) {
        const std::string& key = keyboardEvent->getKey();
        SDL_LogInfo(SDL_LOG_CATEGORY_INPUT, "[EventSystem] Processing keyboard event - Key: %s, Pressed: %d", 
            key.c_str(), keyboardEvent->isPressed());
        
        if (keyboardEvent->isPressed()) {
            SDL_LogWarn(SDL_LOG_CATEGORY_INPUT, "[EventSystem] Key pressed: %s", key.c_str());
            pressedKeys.insert(key);
        } else {
            SDL_LogWarn(SDL_LOG_CATEGORY_INPUT, "[EventSystem] Key released: %s", key.c_str());
            pressedKeys.erase(key);
        }
        
        // Log current pressed keys
        SDL_LogInfo(SDL_LOG_CATEGORY_INPUT, "[EventSystem] Currently pressed keys (%zu):", pressedKeys.size());
        for (const auto& pressedKey : pressedKeys) {
            SDL_LogInfo(SDL_LOG_CATEGORY_INPUT, "  - %s", pressedKey.c_str());
        }
    } else {
        SDL_LogInfo(SDL_LOG_CATEGORY_INPUT, "[EventSystem] Received non-keyboard event");
    }
}

void EventSystem::onEntityAdded(const Entity& entity) {
    SDL_LogInfo(SDL_LOG_CATEGORY_INPUT, "[EventSystem] Checking entity %llu for required components", entity.getId());
    
    // Check if entity has required components
    auto* input = ComponentManager::getInstance().getComponent<components::Input>(entity);
    auto* movement = ComponentManager::getInstance().getComponent<components::Movement>(entity);
    auto* transform = ComponentManager::getInstance().getComponent<components::Transform>(entity);
    
    if (input && movement && transform) {
        SDL_LogInfo(SDL_LOG_CATEGORY_INPUT, "[EventSystem] Entity %llu has all required components, adding to system", entity.getId());
        addEntity(entity);
    } else {
        SDL_LogInfo(SDL_LOG_CATEGORY_INPUT, "[EventSystem] Entity %llu missing required components:", entity.getId());
        if (!input) SDL_LogInfo(SDL_LOG_CATEGORY_INPUT, "  - Missing Input component");
        if (!movement) SDL_LogInfo(SDL_LOG_CATEGORY_INPUT, "  - Missing Movement component");
        if (!transform) SDL_LogInfo(SDL_LOG_CATEGORY_INPUT, "  - Missing Transform component");
    }
}

void EventSystem::onEntityRemoved(const Entity& entity) {
    SDL_LogInfo(SDL_LOG_CATEGORY_INPUT, "[EventSystem] Removing entity %llu from system", entity.getId());
    removeEntity(entity);
}

Vector2 EventSystem::calculateVelocity(const components::Input* input) {
    Vector2 velocity(0, 0);
    float speed = input->getMoveSpeed();
    
    // Log the key bindings being checked
    SDL_LogInfo(SDL_LOG_CATEGORY_INPUT, "[EventSystem] Checking key bindings for entity with speed %.2f:", speed);
    SDL_LogInfo(SDL_LOG_CATEGORY_INPUT, "  - Up: %s", input->getKey("up").c_str());
    SDL_LogInfo(SDL_LOG_CATEGORY_INPUT, "  - Down: %s", input->getKey("down").c_str());
    SDL_LogInfo(SDL_LOG_CATEGORY_INPUT, "  - Left: %s", input->getKey("left").c_str());
    SDL_LogInfo(SDL_LOG_CATEGORY_INPUT, "  - Right: %s", input->getKey("right").c_str());
    
    if (pressedKeys.count(input->getKey("up"))) {
        velocity.y -= speed;
        SDL_LogInfo(SDL_LOG_CATEGORY_INPUT, "[EventSystem] Up key active");
    }
    if (pressedKeys.count(input->getKey("down"))) {
        velocity.y += speed;
        SDL_LogInfo(SDL_LOG_CATEGORY_INPUT, "[EventSystem] Down key active");
    }
    if (pressedKeys.count(input->getKey("left"))) {
        velocity.x -= speed;
        SDL_LogInfo(SDL_LOG_CATEGORY_INPUT, "[EventSystem] Left key active");
    }
    if (pressedKeys.count(input->getKey("right"))) {
        velocity.x += speed;
        SDL_LogInfo(SDL_LOG_CATEGORY_INPUT, "[EventSystem] Right key active");
    }
    
    return velocity;
}

} // namespace systems
} // namespace ecs
} // namespace game 