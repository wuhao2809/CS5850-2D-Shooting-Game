/**
 * ExpiredEntitiesSystem for centralized cleanup of expired entities.
 * Enhanced with request-based entity destruction via DestroyRequest components.
 * 
 * C++ Implementation References:
 * - Python: /Lesson-41-CleanEvents/Python/src/game/ecs/systems/expired_entities_system.py
 * - Java: /Lesson-41-CleanEvents/Java/src/game/ecs/systems/ExpiredEntitiesSystem.java
 * 
 * Implementation of ExpiredEntitiesSystem methods following the exact
 * same patterns established in the Python and Java reference implementations.
 */

#include "ExpiredEntitiesSystem.hpp"
#include "../ComponentManager.hpp"
#include "../SystemManager.hpp"
#include "../components/Expirable.hpp"
#include "../components/DestroyRequest.hpp"
#include <SDL3/SDL.h>
#include <sstream>
#include <algorithm>
#include <unordered_set>

namespace game {
namespace ecs {
namespace systems {

ExpiredEntitiesSystem::ExpiredEntitiesSystem()
    : System()
    , systemManager_(nullptr)
    , ttlDestructions_(0)
    , requestDestructions_(0) {
    
    // Register required components for TTL-based expiration
    registerRequiredComponent<components::Expirable>();
    
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, 
               "[ExpiredEntitiesSystem] Initialized with request-based destruction");
}

void ExpiredEntitiesSystem::setSystemManager(SystemManager* systemManager) {
    systemManager_ = systemManager;
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "[ExpiredEntitiesSystem] System manager reference set");
}

template<typename T>
std::vector<Entity> ExpiredEntitiesSystem::findEntitiesWithComponent() {
    /**
     * Helper method to find all entities with a specific component type.
     * This is a workaround for the lack of a global entity registry.
     */
    std::vector<Entity> entitiesWithComponent;
    ComponentManager& cm = ComponentManager::getInstance();
    
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

void ExpiredEntitiesSystem::update(float deltaTime) {
    /**
     * Check all expirable entities and remove those marked as expired.
     * Enhanced with dual-mode operation:
     * 1. Process DestroyRequest components (new)
     * 2. Process TTL-based entity expiration (preserved)
     */
    std::vector<EntityWithReason> entitiesToRemove;
    
    // NEW: Process DestroyRequest components first
    processDestroyRequests(entitiesToRemove);
    
    // PRESERVED: Process TTL-based entity expiration
    processTtlExpiration(entitiesToRemove);
    
    // Clean up all entities marked for removal
    if (!entitiesToRemove.empty()) {
        cleanupEntities(entitiesToRemove);
    }
    
    SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION,
                "[ExpiredEntitiesSystem] TTL=%d, Requests=%d, Total=%zu",
                ttlDestructions_, requestDestructions_, entitiesToRemove.size());
}

void ExpiredEntitiesSystem::processDestroyRequests(std::vector<EntityWithReason>& entitiesToRemove) {
    /**
     * Process DestroyRequest components for request-based entity destruction.
     * NEW: Request-based entity destruction system.
     */
    
    // Find all entities with DestroyRequest components
    std::vector<Entity> destroyRequestEntities = findEntitiesWithComponent<components::DestroyRequest>();
    
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
               "[ExpiredEntitiesSystem] processDestroyRequests: Found %zu entities with DestroyRequest components",
               destroyRequestEntities.size());
    
    ComponentManager& cm = ComponentManager::getInstance();
    
    for (const Entity& entity : destroyRequestEntities) {
        auto* destroyRequest = cm.getComponent<components::DestroyRequest>(entity);
        
        if (destroyRequest->isProcessed()) {
            SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION,
                        "[ExpiredEntitiesSystem] DestroyRequest for entity %llu already processed",
                        entity.getId());
            continue;
        }
        
        // Check if destruction delay has passed
        if (destroyRequest->isReadyForDestruction()) {
            std::string reason = "request:" + destroyRequest->getReason();
            entitiesToRemove.emplace_back(entity, reason);
            destroyRequest->markProcessed();
            requestDestructions_++;
            
            // Track destruction reason
            const std::string& reasonKey = destroyRequest->getReason();
            destructionReasons_[reasonKey] = destructionReasons_[reasonKey] + 1;
            
            SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
                       "[ExpiredEntitiesSystem] Entity %llu marked for removal via DestroyRequest (reason: %s)",
                       entity.getId(), destroyRequest->getReason().c_str());
        } else {
            float remaining = destroyRequest->getRemainingDelay();
            SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION,
                        "[ExpiredEntitiesSystem] DestroyRequest for entity %llu waiting %.2fs (reason: %s)",
                        entity.getId(), remaining, destroyRequest->getReason().c_str());
        }
    }
}

void ExpiredEntitiesSystem::processTtlExpiration(std::vector<EntityWithReason>& entitiesToRemove) {
    /**
     * Process TTL-based entity expiration (preserved functionality).
     */
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
               "[ExpiredEntitiesSystem] processTtlExpiration: Processing %zu entities with Expirable components",
               getEntities().size());
    
    ComponentManager& cm = ComponentManager::getInstance();
    
    // Check all entities with Expirable component
    for (const Entity& entity : getEntities()) {
        // Get the Expirable component
        auto* expirable = cm.getComponent<components::Expirable>(entity);
        
        // Skip if component is missing (shouldn't happen due to system requirements)
        if (!expirable) {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION,
                       "[ExpiredEntitiesSystem] Entity %llu missing Expirable component",
                       entity.getId());
            continue;
        }
        
        if (expirable->isExpired()) {
            // Check if entity is not already marked for removal by DestroyRequest
            bool alreadyMarked = false;
            for (const auto& marked : entitiesToRemove) {
                if (marked.entity.getId() == entity.getId()) {
                    alreadyMarked = true;
                    break;
                }
            }
            
            if (!alreadyMarked) {
                entitiesToRemove.emplace_back(entity, "ttl:expired");
                ttlDestructions_++;
                SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
                           "[ExpiredEntitiesSystem] Entity %llu marked for removal (TTL expired)",
                           entity.getId());
            }
        }
    }
}

void ExpiredEntitiesSystem::cleanupEntities(const std::vector<EntityWithReason>& entitiesToRemove) {
    /**
     * Remove a list of entities from the game with enhanced debugging.
     */
    ComponentManager& cm = ComponentManager::getInstance();
    std::unordered_map<std::string, int> destructionSummary;
    
    for (const EntityWithReason& entityWithReason : entitiesToRemove) {
        const Entity& entity = entityWithReason.entity;
        const std::string& reason = entityWithReason.reason;
        
        try {
            SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
                       "[ExpiredEntitiesSystem] Cleaning up entity %llu (reason: %s)",
                       entity.getId(), reason.c_str());
            
            // Step 1: Notify SystemManager to remove entity from all systems (if available)
            if (systemManager_) {
                systemManager_->onEntityDestroyed(entity);
                SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
                           "[ExpiredEntitiesSystem] Notified SystemManager about entity %llu removal",
                           entity.getId());
            } else {
                SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION,
                           "[ExpiredEntitiesSystem] SystemManager reference not set, skipping system notification");
            }
            
            // Step 2: Remove all components from ComponentManager
            cm.removeAllComponents(entity);
            SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
                       "[ExpiredEntitiesSystem] Removed all components for entity %llu",
                       entity.getId());
            
            // Track destruction by category for summary
            std::string category = reason.substr(0, reason.find(':'));
            destructionSummary[category] = destructionSummary[category] + 1;
            
            SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION,
                        "[ExpiredEntitiesSystem] Successfully removed entity %llu (reason: %s)",
                        entity.getId(), reason.c_str());
                       
        } catch (const std::exception& e) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                        "[ExpiredEntitiesSystem] Failed to remove entity %llu (reason: %s): %s",
                        entity.getId(), reason.c_str(), e.what());
        } catch (...) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                        "[ExpiredEntitiesSystem] Failed to remove entity %llu (reason: %s): unknown error",
                        entity.getId(), reason.c_str());
        }
    }
    
    // Log destruction summary
    if (!entitiesToRemove.empty()) {
        std::vector<std::string> summaryParts;
        for (const auto& [category, count] : destructionSummary) {
            summaryParts.push_back(category + "=" + std::to_string(count));
        }
        
        std::string summaryStr;
        for (size_t i = 0; i < summaryParts.size(); ++i) {
            if (i > 0) summaryStr += ", ";
            summaryStr += summaryParts[i];
        }
        
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
                   "[ExpiredEntitiesSystem] Cleaned up %zu entities (%s)",
                   entitiesToRemove.size(), summaryStr.c_str());
    }
}

std::unordered_map<std::string, int> ExpiredEntitiesSystem::getStatistics() const {
    /**
     * Get system statistics for debugging.
     * NEW: Statistics tracking for request processing.
     */
    std::unordered_map<std::string, int> stats;
    stats["active_entities"] = static_cast<int>(getEntities().size());
    stats["ttl_destructions"] = ttlDestructions_;
    stats["request_destructions"] = requestDestructions_;
    stats["total_destructions"] = ttlDestructions_ + requestDestructions_;
    
    // Add destruction reasons (flattened for simplicity)
    for (const auto& [reason, count] : destructionReasons_) {
        stats["reason_" + reason] = count;
    }
    
    return stats;
}

void ExpiredEntitiesSystem::resetStatistics() {
    /**
     * Reset destruction statistics (useful for testing).
     * NEW: Statistics management for debugging and testing.
     */
    ttlDestructions_ = 0;
    requestDestructions_ = 0;
    destructionReasons_.clear();
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "[ExpiredEntitiesSystem] Statistics reset");
}

std::string ExpiredEntitiesSystem::toString() const {
    int totalDestroyed = ttlDestructions_ + requestDestructions_;
    std::ostringstream oss;
    oss << "ExpiredEntitiesSystem(entities=" << getEntities().size() 
        << ", destroyed=" << totalDestroyed << ")";
    return oss.str();
}

} // namespace systems
} // namespace ecs
} // namespace game 