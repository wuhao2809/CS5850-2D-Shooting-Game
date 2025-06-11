#pragma once

#include "ComponentManager.hpp"
#include "Entity.hpp"
#include "Component.hpp"
#include <vector>
#include <typeindex>
#include <memory>

namespace game {
namespace ecs {

class System {
public:
    System() : componentManager_(&ComponentManager::getInstance()) {}
    virtual ~System() = default;

    // Update method that systems must implement
    virtual void update(float deltaTime) = 0;

    // Called when an entity is added to the system
    virtual void onEntityAdded(const Entity& entity) = 0;

    // Called when an entity is removed from the system
    virtual void onEntityRemoved(const Entity& entity) = 0;

    // Get all entities managed by this system
    const std::vector<Entity>& getEntities() const { return entities_; }

    // Check if an entity has all required components
    bool hasRequiredComponents(const Entity& entity) const;

protected:
    // Register a required component type
    template<typename T>
    void registerComponent() {
        requiredComponents_.push_back(std::type_index(typeid(T)));
    }

    // Add an entity to the system
    void addEntity(const Entity& entity);

    // Remove an entity from the system
    void removeEntity(const Entity& entity);

    ComponentManager* getComponentManager() const {
        return componentManager_;
    }

    const std::vector<std::type_index>& getRequiredComponents() const {
        return requiredComponents_;
    }

private:
    std::vector<Entity> entities_;  // Maintains insertion order
    std::vector<std::type_index> requiredComponents_;
    ComponentManager* componentManager_;
};

} // namespace ecs
} // namespace game 