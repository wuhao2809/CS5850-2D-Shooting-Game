#include "RenderSystem.hpp"
#include "../../GameWorld.hpp"
#include <algorithm>
#include <cmath>

namespace game::ecs::systems {

// Constructor implementation
RenderSystem::RenderSystem() {
    // Register required components
    registerComponent<components::Transform>();
    registerComponent<components::Sprite>();
}

// Update implementation
void RenderSystem::update(float deltaTime) {
    SDL_Log("[RenderSystem] Update called with deltaTime=%.4f", deltaTime);
    auto* renderer = GameWorld::getInstance().getRenderer();
    SDL_Log("[RenderSystem] Retrieved renderer from GameWorld: %p", renderer);
    
    if (!renderer) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "[RenderSystem] No renderer available!");
        return;
    }

    const auto& entities = getEntities();
    SDL_Log("[RenderSystem] Number of entities in system: %zu", entities.size());
    


    // Render all entities
    for (auto& entity : entities) {
        SDL_Log("[RenderSystem] Processing entity %llu", entity.getId());
        
        auto transform = getComponentManager()->getComponent<components::Transform>(entity);
        auto sprite = getComponentManager()->getComponent<components::Sprite>(entity);
        
        if (!transform || !sprite) {
            SDL_Log("[RenderSystem] Entity %llu missing components - Transform: %p, Sprite: %p", 
                    entity.getId(), transform, sprite);
            continue;
        }
        
        if (!sprite->isVisible()) {
            SDL_Log("[RenderSystem] Entity %llu is not visible", entity.getId());
            continue;
        }

        // Log component data
        SDL_Log("[RenderSystem] Entity %llu data:", entity.getId());
        SDL_Log("  Position: (%.2f, %.2f)", transform->getPosition().x, transform->getPosition().y);
        SDL_Log("  Size: %.2fx%.2f", sprite->getWidth(), sprite->getHeight());
        SDL_Log("  Color: RGBA(%d, %d, %d, %d)", 
                sprite->getColor().r, 
                sprite->getColor().g, 
                sprite->getColor().b, 
                sprite->getColor().a);

        // Create destination rectangle
        SDL_FRect destRect = {
            transform->getPosition().x,
            transform->getPosition().y,
            static_cast<float>(sprite->getWidth()),
            static_cast<float>(sprite->getHeight())
        };

        // Render the sprite
        SDL_Color color = sprite->getColor();
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
        SDL_RenderFillRect(renderer, &destRect);

        // Draw direction line if movement component is available
        auto movement = getComponentManager()->getComponent<components::Movement>(entity);
        if (movement) {
            const auto& vel = movement->getVelocity();
            if (vel.x != 0.0f || vel.y != 0.0f) {
                float length = std::min(sprite->getWidth(), sprite->getHeight()) / 2.0f;
                float centerX = transform->getPosition().x + sprite->getWidth() / 2.0f;
                float centerY = transform->getPosition().y + sprite->getHeight() / 2.0f;
                float mag = std::sqrt(vel.x * vel.x + vel.y * vel.y);
                float dirX = vel.x / mag;
                float dirY = vel.y / mag;
                int endX = static_cast<int>(centerX + dirX * length);
                int endY = static_cast<int>(centerY + dirY * length);
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                SDL_RenderDrawLine(renderer,
                                   static_cast<int>(centerX),
                                   static_cast<int>(centerY),
                                   endX,
                                   endY);
            }
        }

        SDL_Log("[RenderSystem] Rendered entity %llu at (%.2f, %.2f) with size %.2fx%.2f",
                entity.getId(),
                transform->getPosition().x,
                transform->getPosition().y,
                static_cast<float>(sprite->getWidth()),
                static_cast<float>(sprite->getHeight()));
    }

    // Present the rendered frame
    
    SDL_Log("[RenderSystem] Frame presented");
}

void RenderSystem::onEntityAdded(const Entity& entity) {
    // Check if entity has all required components
    if (hasRequiredComponents(entity)) {
        SDL_Log("Entity %llu added to RenderSystem", entity.getId());
        addEntity(entity);
    }
}

void RenderSystem::onEntityRemoved(const Entity& entity) {
    SDL_Log("Entity %llu removed from RenderSystem", entity.getId());
}

} // namespace game::ecs::systems 