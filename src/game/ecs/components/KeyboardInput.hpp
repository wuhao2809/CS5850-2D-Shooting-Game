#pragma once

#include <unordered_set>
#include <string>
#include "game/ecs/Component.hpp"

namespace game {
namespace ecs {
namespace components {

/**
 * KeyboardInput component for frame-perfect input tracking in pure ECS architecture.
 * 
 * C++ Implementation References:
 * - Python: /Lesson-41-CleanEvents/Python/src/game/ecs/components/keyboard_input.py
 * - Java: /Lesson-41-CleanEvents/Java/src/game/ecs/components/KeyboardInput.java
 * 
 * This component provides:
 * - Frame-perfect input detection
 * - Reliable state tracking across frames
 * - Cross-platform input foundation for the pure ECS architecture
 * - Clean separation from event processing logic
 * 
 * Designed to work with UIEventSystem for pure component-based input handling.
 */
class KeyboardInput : public Component {
private:
    // Current input state
    std::unordered_set<std::string> keysPressed;  // Currently held keys
    
    // Component configuration
    bool enabled;                                 // Component enable/disable state

public:
    /**
     * Initialize the KeyboardInput component.
     * 
     * @param entity The entity this component belongs to
     */
    KeyboardInput(const Entity& entity);
    
    /**
     * Check if a key is currently being held down.
     * 
     * @param key Key name to check (e.g., 'a', 'd', ' ')
     * @return true if the key is currently pressed
     */
    bool isPressed(const std::string& key) const;
    
    /**
     * Register a key press event.
     * Called by UIEventSystem when processing input events.
     * 
     * @param key Key that was pressed
     */
    void pressKey(const std::string& key);
    
    /**
     * Register a key release event.
     * Called by UIEventSystem when processing input events.
     * 
     * @param key Key that was released
     */
    void releaseKey(const std::string& key);
    
    /**
     * Check if input processing is enabled for this component.
     * 
     * @return true if input processing is enabled
     */
    bool isEnabled() const;
    
    /**
     * Enable input processing for this component.
     */
    void enable();
    
    /**
     * Disable input processing for this component.
     */
    void disable();
    
    /**
     * Set the enabled state of this component.
     * 
     * @param enabled true to enable, false to disable
     */
    void setEnabled(bool enabled);
    
    /**
     * Get all currently pressed keys.
     * 
     * @return Set of all currently pressed key names (copy for safety)
     */
    std::unordered_set<std::string> getPressedKeys() const;
    
    /**
     * Check if any keys are currently pressed.
     * 
     * @return true if any keys are currently pressed
     */
    bool hasAnyInput() const;
    
    /**
     * Get the number of currently pressed keys.
     * 
     * @return number of currently pressed keys
     */
    size_t getPressedKeyCount() const;
};

} // namespace components
} // namespace ecs
} // namespace game 