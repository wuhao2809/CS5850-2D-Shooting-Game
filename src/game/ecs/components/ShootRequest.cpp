/**
 * ShootRequest component for request-based projectile creation.
 * 
 * C++ Implementation References:
 * - Python: /Lesson-41-CleanEvents/Python/src/game/ecs/components/shoot_request.py
 * - Java: /Lesson-41-CleanEvents/Java/src/game/ecs/components/ShootRequest.java
 * 
 * Implementation of ShootRequest component methods following the exact
 * same patterns established in the Python and Java reference implementations.
 */

#include "ShootRequest.hpp"
#include <sstream>
#include <iomanip>
#include <SDL3/SDL.h>

namespace game {
namespace ecs {
namespace components {

// Constructors
ShootRequest::ShootRequest(Entity entity, float x, float y)
    : Component(entity), position(x, y), processed(false), 
      timestamp(std::chrono::steady_clock::now()) {
    SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, 
                "[ShootRequest] Created component for entity %llu at position (%.1f, %.1f)", 
                entity.getId(), x, y);
}

ShootRequest::ShootRequest(Entity entity, const Vector2& position)
    : Component(entity), position(position), processed(false),
      timestamp(std::chrono::steady_clock::now()) {
    SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, 
                "[ShootRequest] Created component for entity %llu at position (%.1f, %.1f)", 
                entity.getId(), position.x, position.y);
}

ShootRequest::ShootRequest(Entity entity)
    : ShootRequest(entity, 0.0f, 0.0f) {
    SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, 
                "[ShootRequest] Created component for entity %llu with default position", 
                entity.getId());
}

// Methods
void ShootRequest::markProcessed(std::optional<Entity::ID> projectileEntityId) {
    processed = true;
    if (projectileEntityId.has_value()) {
        this->projectileEntityId = projectileEntityId;
        SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION,
                    "[ShootRequest] Marked as processed for entity %llu, created projectile %llu", 
                    getEntity().getId(), projectileEntityId.value());
    } else {
        SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION,
                    "[ShootRequest] Marked as processed for entity %llu", 
                    getEntity().getId());
    }
}

float ShootRequest::getAge() const {
    auto currentTime = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(currentTime - timestamp);
    return duration.count() / 1000000.0f; // Convert microseconds to seconds
}

bool ShootRequest::isStale(float maxAge) const {
    return getAge() > maxAge;
}

std::string ShootRequest::toString() const {
    std::ostringstream oss;
    std::string status = processed ? "processed" : "pending";
    float age = getAge();
    
    oss << "ShootRequest(pos=(" << position.x << ", " << position.y << "), age=" 
        << std::fixed << std::setprecision(2) << age << "s, " << status;
    
    if (projectileEntityId.has_value()) {
        oss << ", projectile_id=" << projectileEntityId.value();
    }
    
    oss << ")";
    return oss.str();
}

} // namespace components
} // namespace ecs
} // namespace game 