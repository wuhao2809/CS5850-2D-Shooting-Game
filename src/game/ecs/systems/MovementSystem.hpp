#pragma once

#include "../System.hpp"
#include "../ComponentManager.hpp"
#include "../Entity.hpp"
#include "../Vector2.hpp"
#include "../components/Transform.hpp"
#include "../components/Movement.hpp"
#include "../components/Sprite.hpp"
#include <SDL3/SDL.h>

namespace game::ecs::systems {

class MovementSystem : public System {
public:
    MovementSystem(int worldWidth, int worldHeight) 
        : worldWidth_(worldWidth), worldHeight_(worldHeight) {
        // Register required components
        registerComponent<components::Transform>();
        registerComponent<components::Movement>();
        registerComponent<components::Sprite>();
    }

    void setWorldSize(int width, int height) {
        worldWidth_ = width;
        worldHeight_ = height;
    }

    void update(float deltaTime) override {
        SDL_Log("[MovementSystem] update triggered, deltaTime=%.4f, entities=%zu", deltaTime, getEntities().size());
        // Log all entity IDs in the set before processing
        std::string ids;
        for (const auto& e : getEntities()) {
            ids += std::to_string(e.getId()) + ", ";
        }
        SDL_Log("[MovementSystem] Entities in system at update: [%s]", ids.c_str());
        for (auto& entity : getEntities()) {
            auto transform = getComponentManager()->getComponent<components::Transform>(entity);
            auto movement = getComponentManager()->getComponent<components::Movement>(entity);
            auto sprite = getComponentManager()->getComponent<components::Sprite>(entity);

            if (!movement->isEnabled()) {
                continue;
            }

            // Log initial state
            SDL_Log("Entity %llu - Initial Position: (%.2f, %.2f), Velocity: (%.2f, %.2f)",
                    entity.getId(),
                    transform->getPosition().x,
                    transform->getPosition().y,
                    movement->getVelocity().x,
                    movement->getVelocity().y);

            // Apply acceleration
            movement->applyAcceleration(deltaTime);

            // Calculate new position
            float newX = transform->getPosition().x + movement->getVelocity().x * deltaTime;
            float newY = transform->getPosition().y + movement->getVelocity().y * deltaTime;

            // Check boundaries and update position
            checkBoundaries(entity, newX, newY, sprite->getWidth(), sprite->getHeight(), movement);
            transform->setPosition(newX, newY);

            // Log final state
            SDL_Log("Entity %llu - Final Position: (%.2f, %.2f), Velocity: (%.2f, %.2f)",
                    entity.getId(),
                    transform->getPosition().x,
                    transform->getPosition().y,
                    movement->getVelocity().x,
                    movement->getVelocity().y);
        }
    }

    void onEntityAdded(const Entity& entity) override {
        SDL_Log("[MovementSystem] onEntityAdded for entity %llu", entity.getId());
        addEntity(entity);
    }

    void onEntityRemoved(const Entity& entity) override {
        SDL_Log("Entity %llu removed from MovementSystem", entity.getId());
    }

private:
    void checkBoundaries(const Entity& entity, float& x, float& y, float width, float height, 
                        components::Movement* movement) {
        bool hitBoundary = false;
        
        // Check horizontal boundaries
        if (x <= 0) {
            x = 0;
            movement->setVelocity(-movement->getVelocity().x, movement->getVelocity().y);
            hitBoundary = true;
        }
        else if (x + width >= worldWidth_) {
            x = worldWidth_ - width;
            movement->setVelocity(-movement->getVelocity().x, movement->getVelocity().y);
            hitBoundary = true;
        }

        // Check vertical boundaries
        if (y <= 0) {
            y = 0;
            movement->setVelocity(movement->getVelocity().x, -movement->getVelocity().y);
            hitBoundary = true;
        }
        else if (y + height >= worldHeight_) {
            y = worldHeight_ - height;
            movement->setVelocity(movement->getVelocity().x, -movement->getVelocity().y);
            hitBoundary = true;
        }

        if (hitBoundary) {
            SDL_Log("Entity %llu hit boundary at (%.2f, %.2f)", entity.getId(), x, y);
        }
    }

    int worldWidth_;
    int worldHeight_;
};

} // namespace game::ecs::systems 