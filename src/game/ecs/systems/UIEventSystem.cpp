#include "game/ecs/systems/UIEventSystem.hpp"
#include "game/ecs/ComponentManager.hpp"
#include <SDL3/SDL.h>
#include <algorithm>

namespace game {
namespace ecs {
namespace systems {

UIEventSystem::UIEventSystem(events::EventManager& eventManager)
    : System(), eventManager_(eventManager) {
    
    // Register required components - only KeyboardInput
    registerRequiredComponent<components::KeyboardInput>();
    
    // Subscribe to existing KeyboardEvent stream
    eventManager_.subscribe("keyboard", this);
    
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "UIEventSystem initialized and subscribed to keyboard events");
}

UIEventSystem::~UIEventSystem() {
    // Unsubscribe from events to prevent dangling pointer
    eventManager_.unsubscribe("keyboard", this);
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "UIEventSystem destroyed and unsubscribed from keyboard events");
}

void UIEventSystem::update(float deltaTime) {
    /**
     * UIEventSystem update method.
     * 
     * Keyboard input is purely event-driven through onEvent().
     * This method intentionally does nothing.
     * 
     * @param deltaTime Time elapsed since last update (unused)
     */
    
    // Log entity count for debugging
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "UIEventSystem update: %zu KeyboardInput entities", getEntities().size());
    
    // Do nothing - keyboard input is purely event-driven
    // All input processing happens in onEvent() when keyboard events are received
}

void UIEventSystem::onEvent(const events::Event& event) {
    /**
     * Handle keyboard events and update KeyboardInput component entities.
     * 
     * This method:
     * 1. Converts platform key events to normalized key strings
     * 2. Updates all KeyboardInput component entities with press/release state
     * 3. Maintains frame-perfect input tracking
     * 
     * @param event The Event to process (should be KeyboardEvent)
     */
    
    // Check if this is a keyboard event
    const events::KeyboardEvent* keyboardEvent = dynamic_cast<const events::KeyboardEvent*>(&event);
    if (!keyboardEvent) {
        return;
    }
    
    // Convert key to normalized string (lowercase for consistency)
    std::string key = keyboardEvent->getKeyText();
    std::transform(key.begin(), key.end(), key.begin(), ::tolower);
    
    // Log that we received an event
    const char* action = keyboardEvent->isPressed() ? "pressed" : "released";
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "UIEventSystem.onEvent: Received '%s' %s for %zu entities",
               key.c_str(), action, getEntities().size());
    
    int updatedEntities = 0;
    ComponentManager* componentManager = getComponentManager();
    
    // Update all entities with KeyboardInput components
    for (const Entity& entity : getEntities()) {
        components::KeyboardInput* keyboardInput = 
            componentManager->getComponent<components::KeyboardInput>(entity);
        
        if (keyboardInput && keyboardInput->isEnabled()) {
            if (keyboardEvent->isPressed()) {
                keyboardInput->pressKey(key);
                SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "UIEventSystem: Key '%s' pressed for entity %llu",
                           key.c_str(), entity.getId());
            } else {
                keyboardInput->releaseKey(key);
                SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "UIEventSystem: Key '%s' released for entity %llu",
                           key.c_str(), entity.getId());
            }
            updatedEntities++;
        } else {
            SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "UIEventSystem: Entity %llu has no enabled KeyboardInput component",
                       entity.getId());
        }
    }
    
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "UIEventSystem: Updated %d/%zu entities with '%s' %s",
               updatedEntities, getEntities().size(), key.c_str(), action);
}

} // namespace systems
} // namespace ecs
} // namespace game 