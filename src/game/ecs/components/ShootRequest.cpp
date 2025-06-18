/**
 * ShootRequest component for request-based projectile creation.
 *
 * C++ Implementation References:
 * - Python:
 * /Lesson-41-CleanEvents/Python/src/game/ecs/components/shoot_request.py
 * - Java: /Lesson-41-CleanEvents/Java/src/game/ecs/components/ShootRequest.java
 *
 * Implementation of ShootRequest component methods following the exact
 * same patterns established in the Python and Java reference implementations.
 */

#include "ShootRequest.hpp"
#include <SDL3/SDL.h>
#include <iomanip>
#include <sstream>

namespace game {
namespace ecs {
namespace components {

// Constructors
ShootRequest::ShootRequest(Entity entity, float x, float y, float dirX,
                           float dirY)
    : Component(entity), position(x, y), direction(dirX, dirY),
      processed(false), timestamp(std::chrono::steady_clock::now()) {
  SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
              "ShootRequest: Created with position (%.1f, %.1f) and direction "
              "(%.2f, %.2f)",
              x, y, dirX, dirY);
}

ShootRequest::ShootRequest(Entity entity, const Vector2 &position,
                           const Vector2 &direction)
    : Component(entity), position(position), direction(direction),
      processed(false), timestamp(std::chrono::steady_clock::now()) {
  SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
              "ShootRequest: Created with position (%.1f, %.1f) and direction "
              "(%.2f, %.2f)",
              position.x, position.y, direction.x, direction.y);
}

ShootRequest::ShootRequest(Entity entity)
    : Component(entity), position(0.0f, 0.0f), direction(0.0f, -1.0f),
      processed(false), timestamp(std::chrono::steady_clock::now()) {
  SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
              "ShootRequest: Created with default position and direction");
}

// Methods
void ShootRequest::markProcessed(std::optional<Entity::ID> projectileEntityId) {
  processed = true;
  if (projectileEntityId.has_value()) {
    this->projectileEntityId = projectileEntityId;
    SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION,
                 "[ShootRequest] Marked as processed for entity %llu, created "
                 "projectile %llu",
                 getEntity().getId(), projectileEntityId.value());
  } else {
    SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION,
                 "[ShootRequest] Marked as processed for entity %llu",
                 getEntity().getId());
  }
}

float ShootRequest::getAge() const {
  auto currentTime = std::chrono::steady_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
      currentTime - timestamp);
  return duration.count() / 1000000.0f; // Convert microseconds to seconds
}

bool ShootRequest::isStale(float maxAge) const { return getAge() > maxAge; }

std::string ShootRequest::toString() const {
  std::ostringstream oss;
  std::string status = processed ? "processed" : "pending";
  float age = getAge();

  oss << "ShootRequest(pos=(" << position.x << ", " << position.y
      << "), age=" << std::fixed << std::setprecision(2) << age << "s, "
      << status;

  if (projectileEntityId.has_value()) {
    oss << ", projectile_id=" << projectileEntityId.value();
  }

  oss << ")";
  return oss.str();
}

} // namespace components
} // namespace ecs
} // namespace game