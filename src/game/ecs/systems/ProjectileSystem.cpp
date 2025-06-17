/**
 * ProjectileSystem for shooting gallery game.
 * Manages projectile lifecycle including distance tracking and expiration marking.
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
 */

#include "ProjectileSystem.hpp"
#include "../ComponentManager.hpp"
#include "../SystemManager.hpp"
#include "../Entity.hpp"
#include "../components/Transform.hpp"
#include "../components/Movement.hpp"
#include "../components/Projectile.hpp"
#include "../components/Target.hpp"
#include "../components/Expirable.hpp"
#include "../components/Collision.hpp"
#include "../components/CollisionResult.hpp"
#include "../components/Sprite.hpp"
#include "../components/ShootRequest.hpp"
#include "../components/ShootingGalleryState.hpp"
#include <cmath>
#include <sstream>
#include <algorithm>
#include <vector>
#include <unordered_set>
#include <optional>

namespace game {
namespace ecs {
namespace systems {

ProjectileSystem::ProjectileSystem()
    : System()
    , systemManager_(nullptr)
    , requestsProcessed_(0)
    , requestsStale_(0) {
    
    // Register required components for projectile entities
    registerRequiredComponent<components::Transform>();
    registerRequiredComponent<components::Movement>();
    registerRequiredComponent<components::Projectile>();
    registerRequiredComponent<components::Expirable>();
    
    // Get system manager reference for proper entity registration
    systemManager_ = &SystemManager::getInstance();
    
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, 
               "[ProjectileSystem] Initialized with pure component-based operation (no events)");
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, 
               "[ProjectileSystem] Required components: Transform, Movement, Projectile, Expirable");
}

ProjectileSystem::~ProjectileSystem() {
    // No event unsubscription needed in pure ECS mode
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, 
               "[ProjectileSystem] Destroyed (pure component-based mode)");
}

template<typename T>
std::vector<Entity> ProjectileSystem::findEntitiesWithComponent() {
    /**
     * Helper method to find all entities with a specific component type.
     * This is a workaround for the lack of a global entity registry.
     */
    std::vector<Entity> entitiesWithComponent;
    ComponentManager& cm = ComponentManager::getInstance();
    
    // Since we don't have a global entity registry, we'll iterate through
    // a reasonable range of entity IDs and check if they have the component
    // This is not ideal but works for the current implementation
    for (Entity::ID id = 0; id < 10000; ++id) {
        Entity entity; // Default constructor creates entity with ID 0
        // We need to create a test entity with this ID
        // Since Entity::create() uses static counter, we can't easily create arbitrary IDs
        // Let's check existing entities from systems instead
    }
    
    // Alternative approach: check entities from all systems
    // This is more reliable than guessing entity IDs
    std::unordered_set<Entity> allEntities;
    
    // Collect entities from all systems via SystemManager
    SystemManager& sm = SystemManager::getInstance();
    for (const auto& system : sm.getSystems()) {
        for (const Entity& entity : system->getEntities()) {
            allEntities.insert(entity);
        }
    }
    
    // Check which ones have the desired component
    for (const Entity& entity : allEntities) {
        if (cm.getComponent<T>(entity)) {
            entitiesWithComponent.push_back(entity);
        }
    }
    
    return entitiesWithComponent;
}

void ProjectileSystem::update(float deltaTime) {
    /**
     * Update projectile behavior including range tracking and request processing.
     * 
     * Pure ECS operation:
     * 1. Process ShootRequest components (request-based creation)
     * 2. Process CollisionResult components (component-based collision handling)
     * 3. Update existing projectile behavior (range tracking)
     */
    
    // Process ShootRequest components first
    processShootRequests();
    
    // Process collision results for projectile-target collisions
    processCollisionResults();
    
    // Update existing projectiles (range tracking, etc.)
    updateProjectiles(deltaTime);
    
    SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, 
                "[ProjectileSystem] update: projectiles=%zu, requests_processed=%d, stale_requests=%d",
                getEntities().size(), requestsProcessed_, requestsStale_);
}

void ProjectileSystem::processShootRequests() {
    /**
     * Process ShootRequest components to create new projectiles.
     * Request-based projectile creation system.
     */
    ComponentManager& cm = ComponentManager::getInstance();
    
    // Find all entities with ShootRequest components
    std::vector<Entity> shootRequestEntities = findEntitiesWithComponent<components::ShootRequest>();
    
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, 
               "[ProjectileSystem] processShootRequests: Found %zu ShootRequest entities",
               shootRequestEntities.size());
    
    for (const Entity& entity : shootRequestEntities) {
        auto* shootRequest = cm.getComponent<components::ShootRequest>(entity);
        
        if (shootRequest->isProcessed()) {
            SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
                       "[ProjectileSystem] ShootRequest from entity %llu already processed",
                       entity.getId());
            continue;
        }
        
        // Check for stale requests
        if (shootRequest->isStale(1.0f)) {
            SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
                       "[ProjectileSystem] Removing stale ShootRequest from entity %llu (age: %.2fs)",
                       entity.getId(), shootRequest->getAge());
            cm.removeComponent<components::ShootRequest>(entity);
            requestsStale_++;
            continue;
        }
        
        // Create projectile from request
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
                   "[ProjectileSystem] Processing ShootRequest from entity %llu at position (%.1f, %.1f)",
                   entity.getId(), shootRequest->getPosition().x, shootRequest->getPosition().y);
        Entity* projectileEntity = createProjectileFromRequest(entity, *shootRequest);
        
        if (projectileEntity) {
            // Mark request as processed
            shootRequest->markProcessed(projectileEntity->getId());
            requestsProcessed_++;
            SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
                       "[ProjectileSystem] Created projectile %llu from ShootRequest at (%.1f, %.1f)",
                       projectileEntity->getId(), 
                       shootRequest->getPosition().x, shootRequest->getPosition().y);
            
            // Remove the processed request (optional - could keep for tracking)
            cm.removeComponent<components::ShootRequest>(entity);
        } else {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION,
                       "[ProjectileSystem] Failed to create projectile from ShootRequest");
        }
    }
    
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
               "[ProjectileSystem] processShootRequests: Processed %d total requests",
               requestsProcessed_);
}

Entity* ProjectileSystem::createProjectileFromRequest(const Entity& requesterEntity, 
                                                    const components::ShootRequest& shootRequest) {
    /**
     * Create a new projectile entity from a ShootRequest.
     * Request-based projectile creation logic.
     */
    try {
        ComponentManager& cm = ComponentManager::getInstance();
        
        // Create new projectile entity
        Entity projectileEntity = Entity::create("projectile");
        
        // Notify system manager about new entity
        systemManager_->onEntityCreated(projectileEntity);
        
        // Add Transform component with position from request
        Vector2 position(shootRequest.getPosition().x, shootRequest.getPosition().y);
        cm.addComponent<components::Transform>(projectileEntity, position);
        systemManager_->onComponentAdded(projectileEntity, typeid(components::Transform));
        
        // Add Movement component (upward movement like original projectiles)
        Vector2 velocity(0, -300); // Same velocity as original projectiles
        cm.addComponent<components::Movement>(projectileEntity, velocity);
        systemManager_->onComponentAdded(projectileEntity, typeid(components::Movement));
        
        // Add Projectile component
        float maxRange = shootRequest.getPosition().y - 10.0f; // Distance to screen top minus buffer
        cm.addComponent<components::Projectile>(projectileEntity, 400.0f, maxRange);
        systemManager_->onComponentAdded(projectileEntity, typeid(components::Projectile));
        
        // Add Collision component
        cm.addComponent<components::Collision>(projectileEntity);
        systemManager_->onComponentAdded(projectileEntity, typeid(components::Collision));
        
        // Add CollisionResult component for collision tracking
        cm.addComponent<components::CollisionResult>(projectileEntity);
        systemManager_->onComponentAdded(projectileEntity, typeid(components::CollisionResult));
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
                   "[ProjectileSystem] Added CollisionResult component to projectile %llu, should register with CollisionSystem",
                   projectileEntity.getId());
        
        // Add Sprite component (same as original projectiles)
        SDL_Color yellowColor = {255, 255, 0, 255};
        cm.addComponent<components::Sprite>(projectileEntity, 4.0f, 10.0f, yellowColor);
        systemManager_->onComponentAdded(projectileEntity, typeid(components::Sprite));
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
                   "[ProjectileSystem] Added Sprite component to projectile %llu",
                   projectileEntity.getId());
        
        // Add Expirable component
        cm.addComponent<components::Expirable>(projectileEntity);
        systemManager_->onComponentAdded(projectileEntity, typeid(components::Expirable));
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
                   "[ProjectileSystem] Added Expirable component to projectile %llu",
                   projectileEntity.getId());
        
        // Get the created projectile component to log max range
        auto* projectileComp = cm.getComponent<components::Projectile>(projectileEntity);
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
                   "[ProjectileSystem] Created projectile %llu with max_range=%.1f",
                   projectileEntity.getId(), projectileComp ? projectileComp->getMaxRange() : 0.0f);
        
        // Store the entity in a static location to return a pointer
        static Entity lastCreatedProjectile;
        lastCreatedProjectile = projectileEntity;
        return &lastCreatedProjectile;
        
    } catch (const std::exception& e) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                    "[ProjectileSystem] Failed to create projectile from request: %s", e.what());
        return nullptr;
    }
}

void ProjectileSystem::processCollisionResults() {
    /**
     * Process CollisionResult components to handle projectile-target collisions.
     * Pure component-based collision handling - no event dependencies.
     */
    ComponentManager& cm = ComponentManager::getInstance();
    
    // Find all entities with CollisionResult components
    std::vector<Entity> collisionEntities = findEntitiesWithComponent<components::CollisionResult>();
    
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
               "[ProjectileSystem] processCollisionResults: Found %zu entities with CollisionResult components",
               collisionEntities.size());
    
    int projectileCollisionEntities = 0;
    int processedCollisions = 0;
    
    for (const Entity& entity : collisionEntities) {
        auto* collisionResult = cm.getComponent<components::CollisionResult>(entity);
        
        if (!collisionResult || collisionResult->isProcessed()) {
            SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
                       "[ProjectileSystem] Entity %llu - collision result null or already processed",
                       entity.getId());
            continue;
        }
        
        // Check if this entity is a projectile
        if (!cm.getComponent<components::Projectile>(entity)) {
            SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION,
                        "[ProjectileSystem] Entity %llu has CollisionResult but is not a projectile",
                        entity.getId());
            continue;
        }
        
        projectileCollisionEntities++;
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
                   "[ProjectileSystem] Processing projectile %llu with %zu collisions",
                   entity.getId(), collisionResult->getCollisions().size());
        
        // Process all collisions for this projectile
        for (const auto& collisionData : collisionResult->getCollisions()) {
            const std::optional<Entity>& otherEntityOpt = collisionData.otherEntity;
            
            SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION,
                        "[ProjectileSystem] Checking collision between projectile %llu and %llu",
                        entity.getId(), otherEntityOpt.has_value() ? otherEntityOpt.value().getId() : 0);
            
            // Check if the other entity is a target
            if (otherEntityOpt.has_value() && cm.getComponent<components::Target>(otherEntityOpt.value())) {
                SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
                           "[ProjectileSystem] PROJECTILE-TARGET COLLISION: %llu hit target %llu",
                           entity.getId(), otherEntityOpt.value().getId());
                handleProjectileTargetCollision(entity, otherEntityOpt.value());
                processedCollisions++;
            } else {
                SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION,
                            "[ProjectileSystem] Other entity %llu is not a target or invalid",
                            otherEntityOpt.has_value() ? otherEntityOpt.value().getId() : 0);
            }
        }
        
        // Mark collision result as processed
        collisionResult->markProcessed();
    }
    
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
               "[ProjectileSystem] processCollisionResults: Processed %d projectile entities, handled %d projectile-target collisions",
               projectileCollisionEntities, processedCollisions);
}

void ProjectileSystem::updateProjectiles(float deltaTime) {
    /**
     * Update existing projectile behavior (preserved functionality).
     * Projectile range tracking and expiration logic.
     */
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, 
               "[ProjectileSystem] updateProjectiles: Processing %zu entities in getEntities()",
               getEntities().size());
    
    ComponentManager& cm = ComponentManager::getInstance();
    
    // Process projectiles
    int processedCount = 0;
    for (const Entity& entity : getEntities()) {
        if (!hasRequiredComponents(entity)) {
            SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
                       "[ProjectileSystem] Entity %llu missing required components",
                       entity.getId());
            continue;
        }
        
        auto* transform = cm.getComponent<components::Transform>(entity);
        auto* movement = cm.getComponent<components::Movement>(entity);
        auto* projectile = cm.getComponent<components::Projectile>(entity);
        auto* expirable = cm.getComponent<components::Expirable>(entity);
        
        // Skip if already marked as expired
        if (expirable->isExpired()) {
            SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
                       "[ProjectileSystem] Entity %llu already expired, skipping",
                       entity.getId());
            continue;
        }
        
        // Calculate distance traveled this frame
        float velocityMagnitude = std::sqrt(
            movement->getVelocity().x * movement->getVelocity().x +
            movement->getVelocity().y * movement->getVelocity().y
        );
        float distanceThisFrame = velocityMagnitude * deltaTime;
        projectile->addTraveledDistance(distanceThisFrame);
        
        SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION,
                    "[ProjectileSystem] Entity %llu at (%.1f, %.1f), traveled=%.1f/%.1f",
                    entity.getId(),
                    transform->getPosition().x, transform->getPosition().y,
                    projectile->getTraveledDistance(), projectile->getMaxRange());
        
        // Check if projectile has exceeded its range
        if (projectile->shouldExpire()) {
            expirable->markExpired();
            SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
                       "[ProjectileSystem] Projectile %llu marked as EXPIRED after traveling %.1f units",
                       entity.getId(), projectile->getTraveledDistance());
        }
        
        processedCount++;
    }
    
    SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION,
                "[ProjectileSystem] updateProjectiles: Processed %d projectiles for range tracking",
                processedCount);
}

void ProjectileSystem::handleProjectileTargetCollision(const Entity& projectileEntity, const Entity& targetEntity) {
    ComponentManager& cm = ComponentManager::getInstance();
    
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
               "[ProjectileSystem] handleProjectileTargetCollision: Processing collision between %llu and %llu",
               projectileEntity.getId(), targetEntity.getId());
    
    // Check if either entity is already expired to prevent duplicate processing
    auto* projectileExpirable = cm.getComponent<components::Expirable>(projectileEntity);
    auto* targetExpirable = cm.getComponent<components::Expirable>(targetEntity);
    
    if ((projectileExpirable && projectileExpirable->isExpired()) || 
        (targetExpirable && targetExpirable->isExpired())) {
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
                   "[ProjectileSystem] Ignoring collision between already expired entities: projectile=%llu, target=%llu",
                   projectileEntity.getId(), targetEntity.getId());
        return;
    }
    
    // Get target component for scoring
    auto* target = cm.getComponent<components::Target>(targetEntity);
    
    // Check if target is already hit to prevent duplicate processing
    if (target->isHitTarget()) {
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
                   "[ProjectileSystem] Target %llu already hit, ignoring collision",
                   targetEntity.getId());
        return;
    }
    
    // Mark target as hit
    target->markAsHit();
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
               "[ProjectileSystem] Marked target %llu as HIT",
               targetEntity.getId());
    
    // Record hit in game state
    auto& gameState = components::ShootingGalleryState::getInstance();
    gameState.addScore(target->getPointValue());
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
               "[ProjectileSystem] Recorded %d points in game state",
               target->getPointValue());
    
    // Mark both entities as expired for removal by ExpiredEntitiesSystem
    if (projectileExpirable) {
        projectileExpirable->markExpired();
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
                   "[ProjectileSystem] Marked projectile %llu as EXPIRED",
                   projectileEntity.getId());
    }
    if (targetExpirable) {
        targetExpirable->markExpired();
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
                   "[ProjectileSystem] Marked target %llu as EXPIRED",
                   targetEntity.getId());
    }
    
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
               "[ProjectileSystem] Projectile hit %s target for %d points (target now marked as HIT)",
               target->getTargetType().c_str(), target->getPointValue());
}

std::string ProjectileSystem::getStatistics() const {
    /**
     * Get system statistics for debugging.
     * Statistics tracking for request processing.
     */
    std::ostringstream oss;
    oss << "ProjectileSystem(entities=" << getEntities().size() 
        << ", requests_processed=" << requestsProcessed_
        << ", requests_stale=" << requestsStale_ << ")";
    return oss.str();
}

std::string ProjectileSystem::toString() const {
    std::ostringstream oss;
    oss << "ProjectileSystem(entities=" << getEntities().size() 
        << ", processed_requests=" << requestsProcessed_ << ")";
    return oss.str();
}

} // namespace systems
} // namespace ecs
} // namespace game 