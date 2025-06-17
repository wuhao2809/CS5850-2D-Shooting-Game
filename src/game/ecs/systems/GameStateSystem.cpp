#include "GameStateSystem.hpp"
#include "../components/Player.hpp"
#include "../components/Projectile.hpp"
#include "../components/Target.hpp"
#include "../ComponentManager.hpp"
#include <stdexcept>

namespace game {
namespace ecs {
namespace systems {

GameStateSystem::GameStateSystem() 
    : System(), state_(GameState::RUNNING) {
    
    // Register required components - ShootingGalleryState for timer updates
    registerRequiredComponent<components::ShootingGalleryState>();
    // Register CollisionResult to get entities with collision data
    registerRequiredComponent<components::CollisionResult>();
    
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "[GameStateSystem] Initialized with pure ECS architecture");
}

void GameStateSystem::update(float deltaTime) {
    // Update the shooting gallery timer for 60-second game duration
    components::ShootingGalleryState& galleryState = components::ShootingGalleryState::getInstance();
    galleryState.updateTimer(deltaTime);
    
    // Check if game ended due to timer
    if (galleryState.isGameOver() && state_ == GameState::RUNNING) {
        state_ = GameState::GAME_OVER;
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "[GameStateSystem] Game Over! Time's up!");
    }
    
    // Process collision results for player collision detection
    processCollisionResults();
}

void GameStateSystem::processCollisionResults() {
    if (state_ != GameState::RUNNING) {
        return; // Don't check collisions if game is already over
    }
    
    ComponentManager& cm = ComponentManager::getInstance();
    
    // Get all entities that have both ShootingGalleryState and CollisionResult components
    for (const Entity& entity : getEntities()) {
        components::CollisionResult* collisionResult = cm.getComponent<components::CollisionResult>(entity);
        if (!collisionResult || collisionResult->getCollisions().empty()) {
            continue;
        }
        
        // Check if this entity is a player
        bool isPlayerEntity = cm.getComponent<components::Player>(entity) != nullptr;
        
        if (isPlayerEntity) {
            // Check each collision for this player entity
            for (const auto& collision : collisionResult->getCollisions()) {
                // Handle the optional Entity properly
                if (!collision.otherEntity.has_value()) {
                    continue; // Skip if no other entity
                }
                Entity otherEntity = collision.otherEntity.value();
                
                // Check if this is a projectile-target collision (should NOT end game)
                bool isProjectileTargetCollision = (
                    (cm.getComponent<components::Projectile>(entity) != nullptr &&
                     cm.getComponent<components::Target>(otherEntity) != nullptr) ||
                    (cm.getComponent<components::Projectile>(otherEntity) != nullptr &&
                     cm.getComponent<components::Target>(entity) != nullptr)
                );
                
                // Only end the game for player collisions, not projectile-target collisions
                if (!isProjectileTargetCollision) {
                    state_ = GameState::GAME_OVER;
                    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "[GameStateSystem] Game Over! Player collision detected!");
                    return; // Exit early once game over is triggered
                }
                // Note: Projectile-target collisions are handled by ProjectileSystem for scoring
            }
        }
    }
}

bool GameStateSystem::isGameOver() const {
    return state_ == GameState::GAME_OVER;
}

void GameStateSystem::reset() {
    state_ = GameState::RUNNING;
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "[GameStateSystem] Game state reset to RUNNING");
}

GameState GameStateSystem::getState() const {
    return state_;
}

} // namespace systems
} // namespace ecs
} // namespace game 