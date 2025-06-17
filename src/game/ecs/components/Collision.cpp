#include "Collision.hpp"

namespace game {
namespace ecs {
namespace components {

Collision::Collision(const Entity& entity) 
    : Component(entity), type_("AABB") {
}

} // namespace components
} // namespace ecs
} // namespace game 