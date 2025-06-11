#pragma once

#include <cstdint>
#include <string>
#include <memory>

namespace game {
namespace ecs {

class Entity {
public:
    using ID = uint64_t;

    // Default constructor needed for unordered_map
    Entity() : id_(0), name_("") {}

    // Create a new entity with a unique ID
    static Entity create(const std::string& name = "") {
        static ID nextId = 0;
        return Entity(nextId++, name);
    }

    // Get the entity's ID
    ID getId() const { return id_; }

    // Get the entity's name
    const std::string& getName() const { return name_; }

    // Set the entity's name
    void setName(const std::string& name) { name_ = name; }

    // Comparison operators
    bool operator==(const Entity& other) const { return id_ == other.id_; }
    bool operator!=(const Entity& other) const { return id_ != other.id_; }
    bool operator<(const Entity& other) const { return id_ < other.id_; }

private:
    // Private constructor to ensure entities are created through create()
    Entity(ID id, const std::string& name) : id_(id), name_(name) {}

    ID id_;
    std::string name_;
};

} // namespace ecs
} // namespace game

// Add hash function for Entity to work with unordered_map
namespace std {
    template<>
    struct hash<game::ecs::Entity> {
        size_t operator()(const game::ecs::Entity& entity) const {
            return hash<game::ecs::Entity::ID>()(entity.getId());
        }
    };
} 