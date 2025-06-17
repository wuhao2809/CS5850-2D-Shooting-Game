/**
 * CollisionResult component for pure component-based collision tracking.
 * 
 * C++ Implementation References:
 * - Python: /Lesson-41-CleanEvents/Python/src/game/ecs/components/collision_result.py
 * - Java: /Lesson-41-CleanEvents/Java/src/game/ecs/components/CollisionResult.java
 * 
 * This component stores collision data in the ECS system for component-based
 * collision handling without event dependencies, providing the foundation 
 * for pure ECS collision architecture.
 */

#pragma once

#include <vector>
#include <chrono>
#include <optional>
#include <string>
#include "../Entity.hpp"
#include "../Component.hpp"
#include "../Vector2.hpp"

namespace game {
namespace ecs {
namespace components {

class CollisionResult : public Component {
public:
    /**
     * Data structure for storing individual collision information.
     * Matches the Python CollisionData class and Java CollisionData nested class exactly.
     */
    struct CollisionData {
        Entity entityA;
        Entity entityB;
        Vector2 collisionPoint;
        Vector2 collisionNormal;
        std::chrono::steady_clock::time_point timestamp;
        std::optional<Entity> otherEntity;  // Will be set by CollisionResult::addCollision()
        std::optional<Entity> owner;        // Will be set by CollisionResult::addCollision()
        
        /**
         * Initialize collision data.
         * 
         * @param entityA First entity involved in collision
         * @param entityB Second entity involved in collision
         * @param collisionPoint Point of collision (Vector2)
         * @param collisionNormal Normalized collision vector (Vector2)
         */
        CollisionData(Entity entityA, Entity entityB, const Vector2& collisionPoint, const Vector2& collisionNormal);
        
        /**
         * Get a string representation of the collision data.
         */
        std::string toString() const;
    };

private:
    std::vector<CollisionData> collisions;
    bool processed;
    int frameCount;
    bool enabled;

public:
    /**
     * Initialize the CollisionResult component.
     * 
     * @param entity The entity this component belongs to
     */
    explicit CollisionResult(Entity entity);
    
    /**
     * Default destructor.
     */
    ~CollisionResult() = default;
    
    // Copy and move constructors/operators
    CollisionResult(const CollisionResult&) = default;
    CollisionResult& operator=(const CollisionResult&) = default;
    CollisionResult(CollisionResult&&) = default;
    CollisionResult& operator=(CollisionResult&&) = default;
    
    /**
     * Add a collision to this component.
     * 
     * @param entityA First entity involved in collision
     * @param entityB Second entity involved in collision
     * @param collisionPoint Point of collision (Vector2)
     * @param collisionNormal Normalized collision vector (Vector2)
     */
    void addCollision(Entity entityA, Entity entityB, const Vector2& collisionPoint, const Vector2& collisionNormal);
    
    /**
     * Get all collision data for this frame.
     * 
     * @return Const reference to vector of CollisionData objects
     */
    const std::vector<CollisionData>& getCollisions() const;
    
    /**
     * Get a copy of all collision data for this frame.
     * 
     * @return Copy of vector of CollisionData objects (for safety)
     */
    std::vector<CollisionData> getCollisionsCopy() const;
    
    /**
     * Check if this entity has any collisions this frame.
     * 
     * @return True if there are unprocessed collisions, false otherwise
     */
    bool hasCollisions() const;
    
    /**
     * Mark all collisions as processed.
     */
    void markProcessed();
    
    /**
     * Clear all collision data for this frame.
     */
    void clearCollisions();
    
    /**
     * Get collision data for collision with specific entity.
     * 
     * @param otherEntity The entity to check collision with
     * @return Pointer to CollisionData if collision exists, nullptr otherwise
     */
    const CollisionData* getCollisionWith(Entity otherEntity) const;
    
    // Getters for component state
    bool isProcessed() const { return processed; }
    int getFrameCount() const { return frameCount; }
    bool isEnabled() const { return enabled; }
    void setEnabled(bool enabled) { this->enabled = enabled; }
    
    /**
     * Get a string representation of this component.
     */
    std::string toString() const;
};

} // namespace components
} // namespace ecs
} // namespace game 