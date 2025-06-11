#include "System.hpp"
#include "ComponentManager.hpp"
#include <algorithm>
#include <typeindex>
#include <SDL3/SDL.h>

namespace game {
namespace ecs {

bool System::hasRequiredComponents(const Entity& entity) const {
    // Check if the entity has all required components
    for (const auto& componentType : requiredComponents_) {
        if (!componentManager_->hasComponent(entity, componentType)) {
            SDL_Log("[System] Entity %llu missing component %s", entity.getId(), componentType.name());
            return false;
        }
    }
    SDL_Log("[System] Entity %llu has all required components", entity.getId());
    return true;
}

void System::addEntity(const Entity& entity) {
    // Check if entity is already in the system
    if (std::find(entities_.begin(), entities_.end(), entity) == entities_.end()) {
        entities_.push_back(entity);
        SDL_Log("[System] Entity %llu added to system", entity.getId());
        // Log all entity IDs in the vector after insertion
        std::string ids;
        for (const auto& e : entities_) {
            ids += std::to_string(e.getId()) + ", ";
        }
        SDL_Log("[System] Entities in system after add: [%s]", ids.c_str());
    } else {
        SDL_Log("[System] Entity %llu already exists in system", entity.getId());
    }
}

void System::removeEntity(const Entity& entity) {
    auto it = std::find(entities_.begin(), entities_.end(), entity);
    if (it != entities_.end()) {
        entities_.erase(it);
        SDL_Log("[System] Entity %llu removed from system", entity.getId());
        onEntityRemoved(entity);
    } else {
        SDL_Log("[System] Entity %llu not found in system", entity.getId());
    }
}

} // namespace ecs
} // namespace game 