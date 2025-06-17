#pragma once

#include "ComponentManager.hpp"
#include "Entity.hpp"
#include "Component.hpp"
#include <vector>
#include <typeindex>
#include <memory>
#include <unordered_set>
#include <SDL3/SDL.h>

namespace game {
namespace ecs {

class System {
public:
    System() : componentManager_(&ComponentManager::getInstance()) {}
    virtual ~System() = default;

    // Required component registration
    template<typename T>
    void registerRequiredComponent() {
        requiredComponents_.insert(std::type_index(typeid(T)));
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "[System] Registered required component: %s", typeid(T).name());
    }

    // Optional component registration
    template<typename T>
    void registerOptionalComponent() {
        optionalComponents_.insert(std::type_index(typeid(T)));
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "[System] Registered optional component: %s", typeid(T).name());
    }

    // Check if entity has all required components
    bool hasRequiredComponents(const Entity& entity) const;

    // Entity management
    void addEntity(const Entity& entity);
    void removeEntity(const Entity& entity);
    const std::vector<Entity>& getEntities() const { return entities_; }

    // Virtual methods to be implemented by derived systems
    virtual void update(float deltaTime) = 0;
    virtual void onEntityAdded(const Entity& entity) {}
    virtual void onEntityRemoved(const Entity& entity) {}

    // Helper method to get optional component
    template<typename T>
    T* getOptionalComponent(const Entity& entity) const {
        if (optionalComponents_.find(std::type_index(typeid(T))) != optionalComponents_.end()) {
            return componentManager_->getComponent<T>(entity);
        }
        return nullptr;
    }

protected:
    ComponentManager* getComponentManager() const {
        return componentManager_;
    }

private:
    std::vector<Entity> entities_;  // Maintains insertion order
    std::unordered_set<std::type_index> requiredComponents_;
    std::unordered_set<std::type_index> optionalComponents_;
    ComponentManager* componentManager_;
};

} // namespace ecs
} // namespace game 