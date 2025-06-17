#pragma once

#include "game/ecs/System.hpp"
#include "game/events/EventListener.hpp"
#include "game/events/EventManager.hpp"
#include "game/events/KeyboardEvent.hpp"
#include "game/ecs/components/KeyboardInput.hpp"
#include <memory>

namespace game {
namespace ecs {
namespace systems {

/**
 * UIEventSystem for processing keyboard events into KeyboardInput component updates.
 * 
 * C++ Implementation References:
 * - Python: /Lesson-41-CleanEvents/Python/src/game/ecs/systems/ui_event_system.py
 * - Java: /Lesson-41-CleanEvents/Java/src/game/ecs/systems/UIEventSystem.java
 * 
 * This system serves as the bridge between platform KeyboardEvents and pure ECS components.
 * 
 * Architecture:
 * Platform Input → GameEngine → KeyboardEvent → UIEventSystem → KeyboardInput Component
 * 
 * This system:
 * 1. Subscribes to the existing keyboard event stream
 * 2. Updates all KeyboardInput component entities with press/release events
 * 3. Provides the foundation for cross-platform input abstraction
 * 4. Maintains frame-perfect input tracking
 */
class UIEventSystem : public System, public events::EventListener {
public:
    /**
     * Creates a new UIEventSystem.
     * 
     * @param eventManager The event manager to subscribe to keyboard events
     */
    UIEventSystem(events::EventManager& eventManager);
    
    /**
     * Destructor - unsubscribes from events
     */
    virtual ~UIEventSystem();

    /**
     * UIEventSystem update method.
     * 
     * Keyboard input is purely event-driven through onEvent().
     * This method intentionally does nothing.
     * 
     * @param deltaTime Time elapsed since last update (unused)
     */
    void update(float deltaTime) override;

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
    void onEvent(const events::Event& event) override;

private:
    events::EventManager& eventManager_;
};

} // namespace systems
} // namespace ecs
} // namespace game 