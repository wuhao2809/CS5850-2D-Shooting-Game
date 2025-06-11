#pragma once

#include <string>
#include <unordered_map>
#include "game/ecs/Component.hpp"

namespace game {
namespace ecs {
namespace components {

class Input : public Component {
private:
    bool enabled;
    float moveSpeed;
    std::unordered_map<std::string, std::string> keys;

public:
    Input(const Entity& entity);
    Input(const Entity& entity, bool enabled, float moveSpeed, const std::unordered_map<std::string, std::string>& keys);

    bool isEnabled() const;
    void setEnabled(bool enabled);
    float getMoveSpeed() const;
    void setMoveSpeed(float speed);
    std::string getKey(const std::string& action) const;
    void setKey(const std::string& action, const std::string& key);
    std::unordered_map<std::string, std::string> getKeys() const;
    void setKeys(const std::unordered_map<std::string, std::string>& keys);
};

} // namespace components
} // namespace ecs
} // namespace game 