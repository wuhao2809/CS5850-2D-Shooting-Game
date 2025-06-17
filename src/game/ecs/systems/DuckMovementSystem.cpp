#include "DuckMovementSystem.hpp"
#include "../ComponentManager.hpp"
#include "../components/Transform.hpp"
#include "../components/Movement.hpp"
#include "../components/Target.hpp"
#include "../components/Expirable.hpp"
#include "../components/Images.hpp"
#include <sstream>

namespace game {
namespace ecs {
namespace systems {

DuckMovementSystem::DuckMovementSystem(float worldWidth, float worldHeight)
    : System()
    , worldWidth_(worldWidth)
    , worldHeight_(worldHeight) {
    
    // Register required components
    registerRequiredComponent<components::Transform>();
    registerRequiredComponent<components::Movement>();
    registerRequiredComponent<components::Target>();
    registerRequiredComponent<components::Expirable>();
    
    // Register optional component for sprite direction
    registerOptionalComponent<components::Images>();
    
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "[DuckMovementSystem] Initialized with world %fx%f", 
                worldWidth_, worldHeight_);
}

void DuckMovementSystem::update(float deltaTime) {
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "[DuckMovementSystem] update: processing %zu ducks", 
               getEntities().size());
    
    ComponentManager& cm = ComponentManager::getInstance();
    
    for (const Entity& entity : getEntities()) {
        // Get required components
        auto* transform = cm.getComponent<components::Transform>(entity);
        auto* movement = cm.getComponent<components::Movement>(entity);
        auto* target = cm.getComponent<components::Target>(entity);
        auto* expirable = cm.getComponent<components::Expirable>(entity);
        
        // Skip if any required component is missing
        if (!transform || !movement || !target || !expirable) {
            continue;
        }
        
        // Skip if movement is disabled or duck is already expired
        if (!movement->isEnabled() || expirable->isExpired()) {
            continue;
        }
        
        // Update position based on velocity
        float oldX = transform->getPosition().x;
        float newX = transform->getPosition().x + movement->getVelocity().x * deltaTime;
        float newY = transform->getPosition().y + movement->getVelocity().y * deltaTime;
        
        // Update position
        transform->setPosition(Vector2(newX, newY));
        
        // Update sprite direction based on velocity if Images component is present
        updateSpriteDirection(entity, movement->getVelocity().x);
        
        // Check if duck has flown off the screen
        // If moving right (positive velocity) and gone off right edge
        if (movement->getVelocity().x > 0 && newX > worldWidth_ + EDGE_MARGIN) {
            SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, 
                       "[DuckMovementSystem] Duck %llu flew off right edge at x=%.1f, marking expired",
                       entity.getId(), newX);
            expirable->markExpired();
        }
        // If moving left (negative velocity) and gone off left edge  
        else if (movement->getVelocity().x < 0 && newX < -DUCK_WIDTH - EDGE_MARGIN) {
            SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, 
                       "[DuckMovementSystem] Duck %llu flew off left edge at x=%.1f, marking expired",
                       entity.getId(), newX);
            expirable->markExpired();
        }
    }
}

void DuckMovementSystem::updateSpriteDirection(const Entity& entity, float velocityX) {
    auto* images = getOptionalComponent<components::Images>(entity);
    if (images && images->getImageCount() >= 2) {
        // Use Images.setCurrentImage(0) for left, (1) for right
        if (velocityX < 0) {
            images->setCurrentImage(0); // Moving left
        } else if (velocityX > 0) {
            images->setCurrentImage(1); // Moving right
        }
    }
}

std::string DuckMovementSystem::toString() const {
    return "DuckMovementSystem(entities=" + std::to_string(getEntities().size()) + ")";
}

} // namespace systems
} // namespace ecs
} // namespace game 