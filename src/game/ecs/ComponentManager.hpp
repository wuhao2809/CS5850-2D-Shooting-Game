#pragma once

#include "Component.hpp"
#include "Entity.hpp"
#include <memory>
#include <typeindex>
#include <unordered_map>
#include <vector>

namespace game {
namespace ecs {

class ComponentManager {
public:
  // Get the singleton instance
  static ComponentManager &getInstance() {
    static ComponentManager instance;
    return instance;
  }

  // Add a component to an entity
  template <typename T, typename... Args>
  void addComponent(const Entity &entity, Args &&...args) {
    auto component = std::make_unique<T>(entity, std::forward<Args>(args)...);
    components_[Component::getTypeId<T>()][entity.getId()] =
        std::move(component);
  }

  // Remove a component from an entity
  template <typename T> void removeComponent(const Entity &entity) {
    auto typeId = Component::getTypeId<T>();
    auto &componentMap = components_[typeId];
    componentMap.erase(entity.getId());
  }

  // Get a component from an entity
  template <typename T> T *getComponent(const Entity &entity) {
    auto typeId = Component::getTypeId<T>();
    auto it = components_.find(typeId);
    if (it != components_.end()) {
      auto &componentMap = it->second;
      auto componentIt = componentMap.find(entity.getId());
      if (componentIt != componentMap.end()) {
        return static_cast<T *>(componentIt->second.get());
      }
    }
    return nullptr;
  }

  // Get all entities with a specific component type
  template <typename T> std::vector<Entity> getEntitiesWithComponent() {
    std::vector<Entity> entities;
    auto typeId = Component::getTypeId<T>();
    auto it = components_.find(typeId);
    if (it != components_.end()) {
      const auto &componentMap = it->second;
      for (const auto &[entityId, component] : componentMap) {
        if (component) {
          entities.push_back(component->getEntity());
        }
      }
    }
    return entities;
  }

  // Check if an entity has a component
  bool hasComponent(const Entity &entity, const std::type_index &typeId) const {
    auto it = components_.find(typeId);
    if (it != components_.end()) {
      const auto &componentMap = it->second;
      return componentMap.find(entity.getId()) != componentMap.end();
    }
    return false;
  }

  // Remove all components for an entity
  void removeAllComponents(const Entity &entity) {
    for (auto &[typeId, componentMap] : components_) {
      componentMap.erase(entity.getId());
    }
  }

  // Reset the component manager (clear all components)
  void reset() { components_.clear(); }

private:
  // Private constructor for singleton
  ComponentManager() = default;

  // Map of component type to map of entity ID to component
  std::unordered_map<std::type_index,
                     std::unordered_map<Entity::ID, std::unique_ptr<Component>>>
      components_;
};

} // namespace ecs
} // namespace game