/**
 * ShootRequest component for request-based projectile creation.
 * 
 * C++ Implementation References:
 * - Python: /Lesson-41-CleanEvents/Python/src/game/ecs/components/shoot_request.py
 * - Java: /Lesson-41-CleanEvents/Java/src/game/ecs/components/ShootRequest.java
 * 
 * This component provides controlled projectile creation with:
 * - Position tracking for spawn location
 * - Processing state management  
 * - Timestamp for stale request cleanup
 * 
 * Part of the hybrid event/ECS architecture enhancement for Phase 3.
 * Enables request-based entity lifecycle management instead of direct creation.
 * 
 * Usage:
 *     // Request projectile creation at specific position
 *     componentManager.addComponent<ShootRequest>(entity, 100.0f, 200.0f);
 *     
 *     // Request projectile creation with Vector2
 *     Vector2 position(100.0f, 200.0f);
 *     componentManager.addComponent<ShootRequest>(entity, position);
 */

#pragma once

#include <chrono>
#include <optional>
#include <string>
#include "../Entity.hpp"
#include "../Component.hpp"
#include "../Vector2.hpp"

namespace game {
namespace ecs {
namespace components {

class ShootRequest : public Component {
private:
    Vector2 position;                                           // Position to create projectile
    bool processed;                                             // Processing state
    std::chrono::steady_clock::time_point timestamp;           // Request creation time
    std::optional<Entity::ID> projectileEntityId;              // Created projectile ID (optional)

public:
    /**
     * Initialize a ShootRequest component with x,y coordinates.
     * 
     * @param entity The entity this component belongs to
     * @param x X coordinate for projectile spawn
     * @param y Y coordinate for projectile spawn
     */
    ShootRequest(Entity entity, float x, float y);
    
    /**
     * Initialize a ShootRequest component with Vector2 position.
     * 
     * @param entity The entity this component belongs to
     * @param position Vector2 position for projectile spawn
     */
    ShootRequest(Entity entity, const Vector2& position);
    
    /**
     * Initialize a ShootRequest component with default position (0,0).
     * 
     * @param entity The entity this component belongs to
     */
    explicit ShootRequest(Entity entity);
    
    /**
     * Default destructor.
     */
    ~ShootRequest() = default;
    
    // Copy and move constructors/operators
    ShootRequest(const ShootRequest&) = default;
    ShootRequest& operator=(const ShootRequest&) = default;
    ShootRequest(ShootRequest&&) = default;
    ShootRequest& operator=(ShootRequest&&) = default;
    
    /**
     * Mark this request as processed.
     * 
     * @param projectileEntityId ID of the created projectile entity (optional)
     */
    void markProcessed(std::optional<Entity::ID> projectileEntityId = std::nullopt);
    
    /**
     * Get the age of this request in seconds.
     * 
     * @return Time elapsed since request creation in seconds
     */
    float getAge() const;
    
    /**
     * Check if this request is stale (too old to process).
     * 
     * @param maxAge Maximum age in seconds before considering stale
     * @return True if request is older than maxAge, false otherwise
     */
    bool isStale(float maxAge = 1.0f) const;
    
    // Getters
    const Vector2& getPosition() const { return position; }
    Vector2 getPositionCopy() const { return position; } // Returns a copy for safety
    std::chrono::steady_clock::time_point getTimestamp() const { return timestamp; }
    bool isProcessed() const { return processed; }
    std::optional<Entity::ID> getProjectileEntityId() const { return projectileEntityId; }
    
    /**
     * Get a string representation of this component.
     */
    std::string toString() const;
};

} // namespace components
} // namespace ecs
} // namespace game 