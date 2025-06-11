#pragma once

#include "Entity.hpp"
#include <typeindex>
#include <memory>

namespace game {
namespace ecs {

class Component {
public:
    virtual ~Component() = default;

    // Get the type ID for a component type
    template<typename T>
    static std::type_index getTypeId() {
        return std::type_index(typeid(T));
    }

    // Get the entity this component belongs to
    const Entity& getEntity() const { return entity_; }

protected:
    // Protected constructor to ensure components are created through derived classes
    Component(const Entity& entity) : entity_(entity) {}

    Entity entity_;
};

// Helper function to create a component
template<typename T, typename... Args>
std::unique_ptr<T> createComponent(const Entity& entity, Args&&... args) {
    return std::make_unique<T>(entity, std::forward<Args>(args)...);
}

} // namespace ecs
} // namespace game 