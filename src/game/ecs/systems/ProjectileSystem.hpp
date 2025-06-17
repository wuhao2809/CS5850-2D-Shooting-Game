/**
 * System that manages projectile behavior including range tracking and expiration marking.
 * Enhanced with request-based projectile creation via ShootRequest components.
 * Pure component-based collision handling via CollisionResult components.
 * 
 * C++ Implementation References:
 * - Python: /Lesson-41-CleanEvents/Python/src/game/ecs/systems/projectile_system.py
 * - Java: /Lesson-41-CleanEvents/Java/src/game/ecs/systems/ProjectileSystem.java
 * 
 * Pure ECS Architecture (Step 3.6):
 * - Range tracking and expiration marking (preserved)
 * - Pure component-based collision handling via CollisionResult components
 * - Request-based projectile creation (preserved)
 * - Stale request cleanup (preserved)
 * - No event dependencies (events removed for pure ECS)
 * 
 * ExpiredEntitiesSystem handles the actual cleanup of expired projectiles.
 * 
 * Based on: Lesson-40-WorldState/Python/src/game/ecs/systems/projectile_system.py
 */

#pragma once

#include "../System.hpp"
#include "../Vector2.hpp"
#include <SDL3/SDL.h>
#include <memory>
#include <string>
#include <vector>

namespace game {
namespace ecs {
    class Entity;
    class SystemManager;
    namespace components {
        class ShootRequest;
    }
namespace systems {

/**
 * System that manages projectile behavior including range tracking and expiration marking.
 * Enhanced with request-based projectile creation via ShootRequest components.
 * Pure component-based collision handling via CollisionResult components.
 */
class ProjectileSystem : public System {
public:
    /**
     * Creates a new ProjectileSystem with pure component-based operation (no events).
     */
    explicit ProjectileSystem();

    /**
     * Virtual destructor for proper cleanup
     */
    virtual ~ProjectileSystem();

    /**
     * Update projectile behavior including range tracking, request processing, and collision handling.
     * 
     * Pure ECS operation:
     * 1. Process ShootRequest components (request-based creation)
     * 2. Process CollisionResult components (component-based collision handling)
     * 3. Update existing projectile behavior (range tracking)
     * 
     * @param deltaTime Time elapsed since last update
     */
    void update(float deltaTime) override;

    /**
     * Get system statistics for debugging.
     * @return Dictionary containing system statistics
     */
    std::string getStatistics() const;

    /**
     * String representation for debugging
     * @return String describing the projectile system
     */
    std::string toString() const;

private:
    /**
     * Helper method to find all entities with a specific component type.
     * This is a workaround for the lack of a global entity registry.
     */
    template<typename T>
    std::vector<Entity> findEntitiesWithComponent();

    /**
     * Process ShootRequest components to create new projectiles.
     * Request-based projectile creation system.
     */
    void processShootRequests();

    /**
     * Create a new projectile entity from a ShootRequest.
     * 
     * @param requesterEntity Entity that made the shoot request
     * @param shootRequest The shoot request component
     * @return The created projectile entity, or nullptr if creation failed
     */
    Entity* createProjectileFromRequest(const Entity& requesterEntity, 
                                       const components::ShootRequest& shootRequest);

    /**
     * Process CollisionResult components to handle projectile-target collisions.
     * Pure component-based collision handling - no event dependencies.
     */
    void processCollisionResults();

    /**
     * Update existing projectile behavior (preserved functionality).
     * @param deltaTime Time elapsed since last update
     */
    void updateProjectiles(float deltaTime);

    /**
     * Handle collision between a projectile and a target using pure component approach.
     * Pure component-based collision handling.
     * 
     * @param projectileEntity The projectile entity
     * @param targetEntity The target entity
     */
    void handleProjectileTargetCollision(const Entity& projectileEntity, const Entity& targetEntity);

    // System manager reference for entity registration
    SystemManager* systemManager_;
    
    // Statistics for request processing
    int requestsProcessed_;
    int requestsStale_;
};

} // namespace systems
} // namespace ecs
} // namespace game 