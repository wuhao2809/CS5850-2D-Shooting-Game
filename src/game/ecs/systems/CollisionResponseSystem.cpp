/**
 * CollisionResponseSystem for processing component-based collision results.
 * 
 * C++ Implementation References:
 * - Python: /Lesson-41-CleanEvents/Python/src/game/ecs/systems/collision_response_system.py
 * - Java: /Lesson-41-CleanEvents/Java/src/game/ecs/systems/CollisionResponseSystem.java
 * 
 * Implementation of CollisionResponseSystem methods following the exact
 * same patterns established in the Python and Java reference implementations.
 */

#include "CollisionResponseSystem.hpp"
#include <sstream>
#include <SDL3/SDL.h>

namespace game {
namespace ecs {
namespace systems {

CollisionResponseSystem::CollisionResponseSystem()
    : componentManager(ComponentManager::getInstance()),
      collisionsProcessed(0),
      entitiesWithCollisions(0) {
    
    // Register required components
    registerRequiredComponent<components::CollisionResult>();
    
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, 
                "[CollisionResponseSystem] Initialized for component-based collision processing");
}

void CollisionResponseSystem::update(float deltaTime) {
    // Reset frame statistics
    int frameCollisions = 0;
    int frameEntities = 0;
    
    // Process all entities with CollisionResult components
    for (const auto& entity : getEntities()) {
        auto* collisionResult = componentManager.getComponent<components::CollisionResult>(entity);
        
        if (collisionResult != nullptr && collisionResult->hasCollisions()) {
            frameEntities++;
            const auto& collisions = collisionResult->getCollisions();
            
            // Process each collision for this entity
            for (const auto& collisionData : collisions) {
                processCollision(entity, collisionData);
                frameCollisions++;
            }
            
            // Mark collisions as processed
            collisionResult->markProcessed();
            SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION,
                        "[CollisionResponseSystem] Processed %zu collisions for entity %llu", 
                        collisions.size(), entity.getId());
        }
    }
    
    // Update statistics
    collisionsProcessed += frameCollisions;
    if (frameEntities > 0) {
        entitiesWithCollisions += frameEntities;
        SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION,
                    "[CollisionResponseSystem] Processed %d collisions across %d entities", 
                    frameCollisions, frameEntities);
    }
}

void CollisionResponseSystem::processCollision(Entity entity, const components::CollisionResult::CollisionData& collisionData) {
    // Example collision processing - in a real game, this would contain
    // specific response logic (damage, destruction, scoring, etc.)
    
    Entity otherEntity = (collisionData.entityA == entity) ? collisionData.entityB : collisionData.entityA;
    
    SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION,
                "[CollisionResponseSystem] Processing collision: Entity %llu collided with Entity %llu", 
                entity.getId(), otherEntity.getId());
    SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION,
                "[CollisionResponseSystem]   Collision point: (%.2f, %.2f)", 
                collisionData.collisionPoint.x, collisionData.collisionPoint.y);
    SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION,
                "[CollisionResponseSystem]   Collision normal: (%.2f, %.2f)", 
                collisionData.collisionNormal.x, collisionData.collisionNormal.y);
    
    // In a real implementation, you might:
    // - Check entity types and apply specific collision rules
    // - Apply damage or effects based on collision data
    // - Trigger animations or sound effects
    // - Update scores or game state
    // - Queue entity destruction or transformation
    
    // For demonstration, we simply log the collision processing
}

std::unordered_map<std::string, int> CollisionResponseSystem::getStatistics() const {
    std::unordered_map<std::string, int> stats;
    stats["total_collisions_processed"] = collisionsProcessed;
    stats["total_entities_with_collisions"] = entitiesWithCollisions;
    stats["entities_registered"] = static_cast<int>(getEntities().size());
    return stats;
}

void CollisionResponseSystem::resetStatistics() {
    collisionsProcessed = 0;
    entitiesWithCollisions = 0;
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "[CollisionResponseSystem] Statistics reset");
}

std::string CollisionResponseSystem::toString() const {
    auto stats = getStatistics();
    std::ostringstream oss;
    oss << "CollisionResponseSystem(entities=" << stats.at("entities_registered")
        << ", collisions_processed=" << stats.at("total_collisions_processed")
        << ", entities_with_collisions=" << stats.at("total_entities_with_collisions") << ")";
    return oss.str();
}

} // namespace systems
} // namespace ecs
} // namespace game 