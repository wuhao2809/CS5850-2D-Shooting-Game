/**
 * ExpiredEntitiesSystem for centralized cleanup of expired entities.
 * Enhanced with request-based entity destruction via DestroyRequest components.
 * 
 * C++ Implementation References:
 * - Python: /Lesson-41-CleanEvents/Python/src/game/ecs/systems/expired_entities_system.py
 * - Java: /Lesson-41-CleanEvents/Java/src/game/ecs/systems/ExpiredEntitiesSystem.java
 * 
 * Features:
 * - TTL-based entity expiration (preserved)
 * - Request-based entity destruction (new)
 * - Enhanced debugging for destruction reasons (new)
 * - Destruction statistics tracking (new)
 * 
 * Processes entities with:
 * - Expirable component (TTL-based expiration)
 * - DestroyRequest component (request-based destruction)
 * 
 * Based on: Lesson-40-WorldState/Python/src/game/ecs/systems/expired_entities_system.py
 */

#pragma once

#include "../System.hpp"
#include "../Entity.hpp"
#include <vector>
#include <string>
#include <unordered_map>

namespace game {
namespace ecs {

// Forward declarations
class SystemManager;

namespace components {
    class DestroyRequest;
}

namespace systems {

/**
 * ExpiredEntitiesSystem for centralized cleanup of expired entities.
 * Enhanced with request-based entity destruction via DestroyRequest components.
 */
class ExpiredEntitiesSystem : public System {
public:
    /**
     * Helper class to track entities with destruction reasons.
     */
    struct EntityWithReason {
        Entity entity;
        std::string reason;
        
        EntityWithReason(Entity entity, const std::string& reason)
            : entity(entity), reason(reason) {}
    };

    /**
     * Constructor
     * Initializes the ExpiredEntitiesSystem with request-based destruction capability.
     */
    ExpiredEntitiesSystem();
    
    /**
     * Copy constructor and assignment operator (deleted for singleton-like behavior)
     */
    ExpiredEntitiesSystem(const ExpiredEntitiesSystem& other) = delete;
    ExpiredEntitiesSystem& operator=(const ExpiredEntitiesSystem& other) = delete;
    
    /**
     * Move constructor and assignment operator
     */
    ExpiredEntitiesSystem(ExpiredEntitiesSystem&& other) noexcept = default;
    ExpiredEntitiesSystem& operator=(ExpiredEntitiesSystem&& other) noexcept = default;
    
    /**
     * Destructor
     */
    ~ExpiredEntitiesSystem() override = default;
    
    /**
     * Set the system manager reference for proper entity removal.
     * This is needed to notify all systems when an entity is removed.
     * 
     * @param systemManager The system manager instance
     */
    void setSystemManager(SystemManager* systemManager);
    
    /**
     * Check all expirable entities and remove those marked as expired.
     * Enhanced with dual-mode operation:
     * 1. Process DestroyRequest components (new)
     * 2. Process TTL-based entity expiration (preserved)
     * 
     * @param deltaTime Time elapsed since last update
     */
    void update(float deltaTime) override;
    
    /**
     * Get system statistics for debugging.
     * NEW: Statistics tracking for request processing.
     * 
     * @return Map containing destruction statistics
     */
    std::unordered_map<std::string, int> getStatistics() const;
    
    /**
     * Reset destruction statistics (useful for testing).
     * NEW: Statistics management for debugging and testing.
     */
    void resetStatistics();
    
    /**
     * String representation for debugging
     * 
     * @return String describing system state
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
     * Process DestroyRequest components for request-based entity destruction.
     * NEW: Request-based entity destruction system.
     * 
     * @param entitiesToRemove List to append entities marked for destruction
     */
    void processDestroyRequests(std::vector<EntityWithReason>& entitiesToRemove);
    
    /**
     * Process TTL-based entity expiration (preserved functionality).
     * 
     * @param entitiesToRemove List to append expired entities
     */
    void processTtlExpiration(std::vector<EntityWithReason>& entitiesToRemove);
    
    /**
     * Remove a list of entities from the game with enhanced debugging.
     * 
     * @param entitiesToRemove Vector of entities with reasons to remove
     */
    void cleanupEntities(const std::vector<EntityWithReason>& entitiesToRemove);
    
    // System manager reference for proper entity removal
    SystemManager* systemManager_;
    
    // Statistics for destruction tracking
    int ttlDestructions_;
    int requestDestructions_;
    std::unordered_map<std::string, int> destructionReasons_;
};

} // namespace systems
} // namespace ecs
} // namespace game 