#include "game/ecs/components/Input.hpp"
#include <algorithm>

namespace game {
namespace ecs {
namespace components {

Input::Input(const Entity& entity) : Component(entity), enabled(true), moveSpeed(200.0f) {
    // Set default key bindings
    keys["up"] = "w";
    keys["down"] = "s";
    keys["left"] = "a";
    keys["right"] = "d";
}

Input::Input(const Entity& entity, bool enabled, float moveSpeed, const std::unordered_map<std::string, std::string>& keys)
    : Component(entity), enabled(enabled), moveSpeed(moveSpeed), keys(keys) {
}

bool Input::isEnabled() const {
    return enabled;
}

void Input::setEnabled(bool enabled) {
    this->enabled = enabled;
}

float Input::getMoveSpeed() const {
    return moveSpeed;
}

void Input::setMoveSpeed(float speed) {
    this->moveSpeed = speed;
}

std::string Input::getKey(const std::string& action) const {
    auto it = keys.find(action);
    return it != keys.end() ? it->second : "";
}

void Input::setKey(const std::string& action, const std::string& key) {
    keys[action] = key;
}

std::unordered_map<std::string, std::string> Input::getKeys() const {
    return keys;
}

void Input::setKeys(const std::unordered_map<std::string, std::string>& keys) {
    this->keys = keys;
}

} // namespace components
} // namespace ecs
} // namespace game 