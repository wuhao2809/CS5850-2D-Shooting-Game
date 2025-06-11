#pragma once

#include "Event.hpp"
#include <string>

namespace game {
namespace events {

/**
 * Represents a keyboard input event in the game.
 * Extends the base Event class with keyboard-specific data.
 */
class KeyboardEvent : public Event {
public:
    /**
     * Initialize a new keyboard event.
     * @param key The key code or SDL key constant
     * @param keyText The text representation of the key
     * @param isPressed Whether the key was pressed (true) or released (false)
     */
    KeyboardEvent(const std::string& key, const std::string& keyText, bool isPressed);

    /**
     * Get the key code.
     * @return The key code or SDL key constant
     */
    const std::string& getKey() const { return key_; }

    /**
     * Get the text representation of the key.
     * @return The key text
     */
    const std::string& getKeyText() const { return keyText_; }

    /**
     * Check if the key was pressed.
     * @return true if the key was pressed, false if released
     */
    bool isPressed() const { return isPressed_; }

private:
    const std::string key_;
    const std::string keyText_;
    const bool isPressed_;
};

} // namespace events
} // namespace game 