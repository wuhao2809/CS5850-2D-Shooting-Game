#pragma once

#include "System.hpp"
#include "Entity.hpp"
#include <vector>
#include <memory>
#include <unordered_map>
#include <SDL3/SDL.h>

namespace game {
namespace ecs {

class SystemManager {
public:
    // Get the singleton instance
    static SystemManager& getInstance() {
        static SystemManager instance;
        return instance;
    }

    // Add a system
    template<typename T, typename... Args>
    T* addSystem(Args&&... args) {
        // Check if we already have a system of this type
        for (const auto& system : systems_) {
            if (dynamic_cast<T*>(system.get())) {
                SDL_Log("[SystemManager] System of type %s already exists", typeid(T).name());
                return dynamic_cast<T*>(system.get());
            }
        }

        // Create and add the new system
        auto system = std::make_unique<T>(std::forward<Args>(args)...);
        T* systemPtr = system.get();
        SDL_Log("[SystemManager] Adding system: %s", typeid(T).name());
        systems_.push_back(std::move(system));

        // Note: We don't need to register with existing entities here
        // because entities will be added to systems when they are created
        // or when their components are added

        return systemPtr;
    }

    // Get a system by type
    template<typename T>
    T* getSystem() {
        for (auto& system : systems_) {
            if (auto* typedSystem = dynamic_cast<T*>(system.get())) {
                return typedSystem;
            }
        }
        return nullptr;
    }

    // Expose systems for inspection
    const std::vector<std::unique_ptr<System>>& getSystems() const { return systems_; }

    // Update all systems
    void update(float deltaTime) {
        SDL_Log("[SystemManager] update all systems, count=%zu, deltaTime=%.4f", systems_.size(), deltaTime);
        for (auto& system : systems_) {
            size_t entityCount = system->getEntities().size();
            SDL_Log("[SystemManager] System %s has %zu entities", getSystemName(system.get()), entityCount);
            system->update(deltaTime);
        }
    }

    // Handle entity creation
    void onEntityCreated(const Entity& entity) {
        SDL_Log("[SystemManager] onEntityCreated for entity %llu", entity.getId());
        for (auto& system : systems_) {
            if (system->hasRequiredComponents(entity)) {
                system->onEntityAdded(entity);
            }
        }
    }

    // Handle entity destruction
    void onEntityDestroyed(const Entity& entity) {
        SDL_Log("[SystemManager] onEntityDestroyed for entity %llu", entity.getId());
        for (auto& system : systems_) {
            system->onEntityRemoved(entity);
        }
    }

    // Handle component addition
    void onComponentAdded(const Entity& entity, const std::type_index& componentType) {
        SDL_Log("[SystemManager] onComponentAdded for entity %llu, component: %s", 
                entity.getId(), componentType.name());
        for (auto& system : systems_) {
            if (system->hasRequiredComponents(entity)) {
                system->onEntityAdded(entity);
            }
        }
    }

private:
    // Private constructor for singleton
    SystemManager() = default;

    // Helper method to get system name
    const char* getSystemName(const System* system) const {
        return typeid(*system).name();
    }

    // Vector of systems
    std::vector<std::unique_ptr<System>> systems_;
};

} // namespace ecs
} // namespace game 