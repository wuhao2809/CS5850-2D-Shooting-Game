/**
 * CollisionResponseSystem for processing component-based collision results.
 * 
 * C++ Implementation References:
 * - Python: /Lesson-41-CleanEvents/Python/src/game/ecs/systems/collision_response_system.py
 * - Java: /Lesson-41-CleanEvents/Java/src/game/ecs/systems/CollisionResponseSystem.java
 * 
 * This system demonstrates how to process collision data stored in CollisionResult components,
 * providing an alternative to event-based collision handling while working alongside existing systems.
 * 
 * This system demonstrates the component-based collision architecture by:
 * 1. Processing entities with CollisionResult components
 * 2. Analyzing collision data for each entity
 * 3. Marking collisions as processed
 * 4. Providing foundation for component-based collision response
 * 
 * Works alongside existing event-based collision systems for dual-mode operation.
 */

#pragma once

#include <unordered_map>
#include <string>
#include "../System.hpp"
#include "../Entity.hpp"
#include "../ComponentManager.hpp"
#include "../components/CollisionResult.hpp"

namespace game {
namespace ecs {
namespace systems {

class CollisionResponseSystem : public System {
private:
    ComponentManager& componentManager;
    
    // Statistics tracking
    int collisionsProcessed;
    int entitiesWithCollisions;
    
    /**
     * Process a single collision for an entity.
     * 
     * This is where specific collision response logic would be implemented.
     * For now, it demonstrates the component-based collision processing pattern.
     * 
     * @param entity The entity that experienced the collision
     * @param collisionData The CollisionData containing collision information
     */
    void processCollision(Entity entity, const components::CollisionResult::CollisionData& collisionData);

public:
    /**
     * Initialize the CollisionResponseSystem.
     */
    CollisionResponseSystem();
    
    /**
     * Default destructor.
     */
    ~CollisionResponseSystem() = default;
    
    // Delete copy constructor and assignment operator for singleton-like behavior
    CollisionResponseSystem(const CollisionResponseSystem&) = delete;
    CollisionResponseSystem& operator=(const CollisionResponseSystem&) = delete;
    
    /**
     * Update the collision response system.
     * 
     * Processes all entities with CollisionResult components that have unprocessed collisions.
     * This demonstrates component-based collision handling as an alternative to events.
     * 
     * @param deltaTime Time elapsed since last update
     */
    void update(float deltaTime) override;
    
    /**
     * Get collision processing statistics.
     * 
     * @return Map containing statistics about collision processing
     */
    std::unordered_map<std::string, int> getStatistics() const;
    
    /**
     * Reset collision processing statistics.
     */
    void resetStatistics();
    
    /**
     * Get a string representation of this system.
     */
    std::string toString() const;
};

} // namespace systems
} // namespace ecs
} // namespace game 