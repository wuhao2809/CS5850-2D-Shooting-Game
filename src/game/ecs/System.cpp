#include "System.hpp"
#include "ComponentManager.hpp"
#include <algorithm>
#include <typeindex>
#include <SDL3/SDL.h>

namespace game {
namespace ecs {

bool System::hasRequiredComponents(const Entity& entity) const {
    if (!componentManager_) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "[System] ComponentManager not set for system %s", 
            typeid(*this).name());
        return false;
    }

    // Check required components
    for (const auto& componentType : requiredComponents_) {
        if (!componentManager_->hasComponent(entity, componentType)) {
            SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "[System] Entity %llu missing required component %s for system %s", 
                entity.getId(), componentType.name(), typeid(*this).name());
            return false;
        }
    }

    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "[System] Entity %llu has all required components for system %s", 
        entity.getId(), typeid(*this).name());
    return true;
}

void System::addEntity(const Entity& entity) {
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "[System] Adding entity %llu to system %s", 
        entity.getId(), typeid(*this).name());
    
    // Check if entity already exists
    for (const auto& existingEntity : entities_) {
        if (existingEntity.getId() == entity.getId()) {
            SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "[System] Entity %llu already exists in system %s", 
                entity.getId(), typeid(*this).name());
            return;
        }
    }
    
    entities_.push_back(entity);
    onEntityAdded(entity);
    
    // Log all entities in the system
    std::string entityIds;
    for (const auto& e : entities_) {
        entityIds += std::to_string(e.getId()) + ", ";
    }
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "[System] Entities in system %s after add: [%s]", 
        typeid(*this).name(), entityIds.c_str());
}

void System::removeEntity(const Entity& entity) {
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "[System] Removing entity %llu from system %s", 
        entity.getId(), typeid(*this).name());
    
    auto it = std::find_if(entities_.begin(), entities_.end(),
        [&entity](const Entity& e) { return e.getId() == entity.getId(); });
    
    if (it != entities_.end()) {
        entities_.erase(it);
        onEntityRemoved(entity);
        
        // Log all entities in the system
        std::string entityIds;
        for (const auto& e : entities_) {
            entityIds += std::to_string(e.getId()) + ", ";
        }
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "[System] Entities in system %s after remove: [%s]", 
            typeid(*this).name(), entityIds.c_str());
    } else {
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "[System] Entity %llu not found in system %s", 
            entity.getId(), typeid(*this).name());
    }
}

} // namespace ecs
} // namespace game 