#pragma once
#include "../Component.hpp"
#include "../Entity.hpp"
#include <string>

namespace game {
namespace ecs {
namespace components {

class Collision : public Component {
public:
    Collision(const Entity& entity);
    
    const std::string& getType() const { return type_; }
    void setType(const std::string& type) { type_ = type; }

private:
    std::string type_;  // Currently only supporting "AABB"
};

} // namespace components
} // namespace ecs
} // namespace game 