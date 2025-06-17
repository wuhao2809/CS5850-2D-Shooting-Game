#include "game/ecs/components/KeyboardInput.hpp"
#include <algorithm>

namespace game {
namespace ecs {
namespace components {

KeyboardInput::KeyboardInput(const Entity& entity) 
    : Component(entity), enabled(true) {
    // Initialize with empty key state
    keysPressed.clear();
}

bool KeyboardInput::isPressed(const std::string& key) const {
    std::string lowerKey = key;
    std::transform(lowerKey.begin(), lowerKey.end(), lowerKey.begin(), ::tolower);
    return keysPressed.find(lowerKey) != keysPressed.end();
}

void KeyboardInput::pressKey(const std::string& key) {
    std::string lowerKey = key;
    std::transform(lowerKey.begin(), lowerKey.end(), lowerKey.begin(), ::tolower);
    keysPressed.insert(lowerKey);
}

void KeyboardInput::releaseKey(const std::string& key) {
    std::string lowerKey = key;
    std::transform(lowerKey.begin(), lowerKey.end(), lowerKey.begin(), ::tolower);
    keysPressed.erase(lowerKey);
}

bool KeyboardInput::isEnabled() const {
    return enabled;
}

void KeyboardInput::enable() {
    enabled = true;
}

void KeyboardInput::disable() {
    enabled = false;
}

void KeyboardInput::setEnabled(bool enabled) {
    this->enabled = enabled;
}

std::unordered_set<std::string> KeyboardInput::getPressedKeys() const {
    return keysPressed;  // Return copy for safety
}

bool KeyboardInput::hasAnyInput() const {
    return !keysPressed.empty();
}

size_t KeyboardInput::getPressedKeyCount() const {
    return keysPressed.size();
}

} // namespace components
} // namespace ecs
} // namespace game 