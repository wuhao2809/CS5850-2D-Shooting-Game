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
 * System that handles entity movement with boundary bouncing.
 * 
 * Based on: Lesson-40-WorldState/Python/src/game/ecs/systems/movement_with_bounce_system.py
 *           Lesson-40-WorldState/Java/src/game/ecs/systems/MovementWithBounceSystem.java
 */
class MovementWithBounceSystem : public System {
public:
    /**
     * Constructor
     * @param worldWidth Width of the game world
     * @param worldHeight Height of the game world  
     */
    MovementWithBounceSystem(int worldWidth, int worldHeight) 
        : worldWidth_(worldWidth), worldHeight_(worldHeight) {
        // Register required components
        registerRequiredComponent<components::Transform>();
        registerRequiredComponent<components::Movement>();
        registerRequiredComponent<components::Sprite>();
        
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, 
                   "MovementWithBounceSystem initialized with world size: %dx%d", 
                   worldWidth, worldHeight);
    }

    /**
     * Set world dimensions
     */
    void setWorldSize(int width, int height) {
        worldWidth_ = width;
        worldHeight_ = height;
    }

    /**
     * Update all entities with movement
     */
    void update(float deltaTime) override {
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, 
                   "MovementWithBounceSystem update called with deltaTime: %.4f", deltaTime);
        
        for (auto& entity : getEntities()) {
            processEntity(entity, deltaTime);
        }
    }

    void onEntityAdded(const Entity& entity) override {
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, 
                   "[MovementWithBounceSystem] onEntityAdded for entity %llu", entity.getId());
        addEntity(entity);
    }

    void onEntityRemoved(const Entity& entity) override {
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, 
                   "Entity %llu removed from MovementWithBounceSystem", entity.getId());
    }

private:
    /**
     * Process a single entity's movement
     */
    void processEntity(const Entity& entity, float deltaTime) {
        auto transform = getComponentManager()->getComponent<components::Transform>(entity);
        auto movement = getComponentManager()->getComponent<components::Movement>(entity);
        auto sprite = getComponentManager()->getComponent<components::Sprite>(entity);

        if (!movement->isEnabled()) {
            SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, 
                       "Entity %llu movement is disabled", entity.getId());
            return;
        }

        // Log initial state
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, 
                   "Entity %llu movement update:", entity.getId());
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, 
                   "  Initial position: (%.2f, %.2f)", 
                   transform->getPosition().x, transform->getPosition().y);
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, 
                   "  Initial velocity: (%.2f, %.2f)", 
                   movement->getVelocity().x, movement->getVelocity().y);

        // Update velocity based on acceleration
        movement->applyAcceleration(deltaTime);

        // Apply max speed limit if set
        if (movement->getMaxSpeed() > 0) {
            clampVelocity(movement);
        }

        // Calculate new position
        float newX = transform->getPosition().x + movement->getVelocity().x * deltaTime;
        float newY = transform->getPosition().y + movement->getVelocity().y * deltaTime;

        // Check boundaries and update position (with bouncing)
        checkBoundaries(entity, newX, newY, sprite->getWidth(), sprite->getHeight(), movement);
        
        // Update position
        transform->setPosition(newX, newY);

        // Log final state
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, 
                   "  Final position: (%.2f, %.2f)", 
                   transform->getPosition().x, transform->getPosition().y);
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, 
                   "  Final velocity: (%.2f, %.2f)", 
                   movement->getVelocity().x, movement->getVelocity().y);
    }

    /**
     * Check and handle boundary collisions for an entity
     */
    void checkBoundaries(const Entity& entity, float& x, float& y, float width, float height, 
                        components::Movement* movement) {
        bool hitBoundary = false;
        
        // Check horizontal boundaries  
        if (x < 0) {
            x = 0;
            movement->setVelocity(abs(movement->getVelocity().x), movement->getVelocity().y);
            hitBoundary = true;
        } else if (x + width > worldWidth_) {
            x = worldWidth_ - width;
            movement->setVelocity(-abs(movement->getVelocity().x), movement->getVelocity().y);
            hitBoundary = true;
        }
            
        // Check vertical boundaries
        if (y < 0) {
            y = 0;
            movement->setVelocity(movement->getVelocity().x, abs(movement->getVelocity().y));
            hitBoundary = true;
        } else if (y + height > worldHeight_) {
            y = worldHeight_ - height;
            movement->setVelocity(movement->getVelocity().x, -abs(movement->getVelocity().y));
            hitBoundary = true;
        }
            
        if (hitBoundary) {
            SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, 
                       "Entity %llu hit boundary at (%.2f, %.2f)", entity.getId(), x, y);
        }
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

    int worldWidth_;
    int worldHeight_;
};

} // namespace game::ecs::systems 