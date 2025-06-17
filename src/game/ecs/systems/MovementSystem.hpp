#pragma once

#include "../System.hpp"
#include "../ComponentManager.hpp"
#include "../Entity.hpp"
#include "../Vector2.hpp"
#include "../components/Transform.hpp"
#include "../components/Movement.hpp"
#include "../components/Sprite.hpp"
#include <SDL3/SDL.h>
#include <memory>

namespace game::ecs::systems {

/**
 * System that handles simple entity movement without boundary collision.
 * 
 * Based on: Lesson-40-WorldState/Python/src/game/ecs/systems/movement_system.py
 *           Lesson-40-WorldState/Java/src/game/ecs/systems/MovementSystem.java
 */
class MovementSystem : public System {
public:
    /**
     * Constructor - no world dimensions needed since no boundary checking
     */
    MovementSystem() {
        // Register required components
        registerRequiredComponent<components::Transform>();
        registerRequiredComponent<components::Movement>();
        registerRequiredComponent<components::Sprite>();
        
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, 
                   "MovementSystem initialized (no boundary collision)");
    }

    /**
     * Update all entities with movement
     */
    void update(float deltaTime) override {
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, 
                   "[MovementSystem] update triggered, deltaTime=%.4f, entities=%zu", 
                   deltaTime, getEntities().size());
        
        // Log all entity IDs in the set before processing
        std::string ids;
        for (const auto& e : getEntities()) {
            ids += std::to_string(e.getId()) + ", ";
        }
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, 
                   "[MovementSystem] Entities in system at update: [%s]", ids.c_str());
        
        for (auto& entity : getEntities()) {
            processEntity(entity, deltaTime);
        }
    }

    void onEntityAdded(const Entity& entity) override {
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, 
                   "[MovementSystem] onEntityAdded for entity %llu", entity.getId());
        addEntity(entity);
    }

    void onEntityRemoved(const Entity& entity) override {
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, 
                   "Entity %llu removed from MovementSystem", entity.getId());
    }

private:
    /**
     * Process a single entity's movement
     */
    void processEntity(const Entity& entity, float deltaTime) {
        auto transform = getComponentManager()->getComponent<components::Transform>(entity);
        auto movement = getComponentManager()->getComponent<components::Movement>(entity);

        if (!movement->isEnabled()) {
            SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, 
                       "Entity %llu movement is disabled", entity.getId());
            return;
        }

        // Log initial state
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, 
                   "Entity %llu - Initial Position: (%.2f, %.2f), Velocity: (%.2f, %.2f)",
                   entity.getId(),
                   transform->getPosition().x,
                   transform->getPosition().y,
                   movement->getVelocity().x,
                   movement->getVelocity().y);

        // Update velocity based on acceleration
        movement->applyAcceleration(deltaTime);

        // Apply max speed limit if set
        if (movement->getMaxSpeed() > 0) {
            clampVelocity(movement);
        }

        // Calculate and apply new position (no boundary checking)
        float newX = transform->getPosition().x + movement->getVelocity().x * deltaTime;
        float newY = transform->getPosition().y + movement->getVelocity().y * deltaTime;
        transform->setPosition(newX, newY);

        // Log final state
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, 
                   "Entity %llu - Final Position: (%.2f, %.2f), Velocity: (%.2f, %.2f)",
                   entity.getId(),
                   transform->getPosition().x,
                   transform->getPosition().y,
                   movement->getVelocity().x,
                   movement->getVelocity().y);
    }

    /**
     * Clamp velocity to max speed if it exceeds the limit
     */
    void clampVelocity(components::Movement* movement) {
        auto velocity = movement->getVelocity();
        float speed = sqrt(velocity.x * velocity.x + velocity.y * velocity.y);
        
        if (speed > movement->getMaxSpeed()) {
            float scale = movement->getMaxSpeed() / speed;
            movement->setVelocity(velocity.x * scale, velocity.y * scale);
        }
    }
};

} // namespace game::ecs::systems 