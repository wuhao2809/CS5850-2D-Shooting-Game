#pragma once

#include "../System.hpp"
#include "../Vector2.hpp"
#include "../ComponentManager.hpp"
#include "../SystemManager.hpp"
#include <SDL3/SDL.h>
#include <memory>

namespace game {
namespace ecs {
    class Entity;
namespace systems {

/**
 * System that detects collisions between entities using pure ECS architecture.
 * 
 * C++ Implementation References:
 * - Python: /Lesson-41-CleanEvents/Python/src/game/ecs/systems/collision_system.py
 * - Java: /Lesson-41-CleanEvents/Java/src/game/ecs/systems/CollisionSystem.java
 * 
 * Pure ECS Implementation:
 * 1. Stores collision results in CollisionResult components only
 * 2. No event dependencies - component-based collision handling only
 * 3. Uses AABB collision detection algorithm
 * 
 * Uses AABB (Axis-Aligned Bounding Box) collision detection.
 * Requires entities to have Transform, Sprite, and Collision components.
 */
class CollisionSystem : public System {
public:
    /**
     * Creates a new CollisionSystem with pure ECS architecture.
     */
    explicit CollisionSystem();

    /**
     * Virtual destructor for proper cleanup
     */
    virtual ~CollisionSystem() = default;

    /**
     * Update the collision system.
     * Checks for collisions between all entities with the required components
     * and stores collision results in CollisionResult components.
     * @param deltaTime Time elapsed since last update
     */
    void update(float deltaTime) override;

    /**
     * String representation for debugging
     * @return String describing the collision system
     */
    std::string toString() const;

private:
    /**
     * Information about a collision between two entities.
     */
    struct CollisionInfo {
        Vector2 collisionPoint;
        Vector2 collisionNormal;
        
        CollisionInfo(const Vector2& point, const Vector2& normal)
            : collisionPoint(point), collisionNormal(normal) {}
    };

    /**
     * Check if two entities are colliding using AABB collision detection.
     * @param entityA First entity to check
     * @param entityB Second entity to check
     * @return true if the entities are colliding, false otherwise
     */
    bool checkCollision(const Entity& entityA, const Entity& entityB);

    /**
     * Calculate the collision point and normal between two entities.
     * @param entityA First entity
     * @param entityB Second entity
     * @return CollisionInfo containing collision point and normal
     */
    CollisionInfo calculateCollisionInfo(const Entity& entityA, const Entity& entityB);

    /**
     * Clear all collision results for a fresh detection cycle.
     * Component-based collision result management.
     */
    void clearCollisionResults();

    /**
     * Store collision results in both entities' CollisionResult components.
     * Pure component-based collision storage.
     * @param entityA First entity in collision
     * @param entityB Second entity in collision
     */
    void storeCollisionResult(const Entity& entityA, const Entity& entityB);

    /**
     * Ensure an entity has a CollisionResult component, creating one if needed.
     * Dynamic component creation for collision tracking.
     * @param entity The entity to check/create CollisionResult component for
     */
    void ensureCollisionResultComponent(const Entity& entity);

    // Manager references
    ComponentManager& componentManager_;
    SystemManager& systemManager_;
};

} // namespace systems
} // namespace ecs
} // namespace game 