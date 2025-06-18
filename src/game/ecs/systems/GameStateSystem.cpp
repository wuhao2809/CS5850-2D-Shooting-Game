#include "GameStateSystem.hpp"
#include "../ComponentManager.hpp"
#include "../SystemManager.hpp"
#include "../components/CollisionResult.hpp"
#include "../components/Player.hpp"
#include "../components/Projectile.hpp"
#include "../components/Target.hpp"
#include <stdexcept>
#include <unordered_set>

namespace game {
namespace ecs {
namespace systems {

GameStateSystem::GameStateSystem() : System(), state_(GameState::RUNNING) {

  // Don't register any required components
  // This system needs to check ALL entities globally

  SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
              "[GameStateSystem] Initialized with pure ECS architecture");
}

void GameStateSystem::update(float deltaTime) {
  // Update the shooting gallery timer for 60-second game duration
  components::ShootingGalleryState &galleryState =
      components::ShootingGalleryState::getInstance();
  galleryState.updateTimer(deltaTime);

  // Check if game ended due to timer
  if (galleryState.isGameOver() && state_ == GameState::RUNNING) {
    state_ = GameState::GAME_OVER;
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
                "[GameStateSystem] Game Over! Time's up!");
  }

  // Process collision results for player collision detection
  processCollisionResults();
}

void GameStateSystem::processCollisionResults() {
  if (state_ != GameState::RUNNING) {
    return; // Don't check collisions if game is already over
  }

  ComponentManager &cm = ComponentManager::getInstance();

  // Find ALL entities in the game by collecting from all systems
  std::unordered_set<Entity> allEntities;

  SystemManager &sm = SystemManager::getInstance();
  for (const auto &system : sm.getSystems()) {
    for (const Entity &entity : system->getEntities()) {
      allEntities.insert(entity);
    }
  }

  // Check each entity for collision results
  for (const Entity &entity : allEntities) {
    components::CollisionResult *collisionResult =
        cm.getComponent<components::CollisionResult>(entity);

    if (!collisionResult || collisionResult->getCollisions().empty()) {
      continue;
    }

    // Check if this entity is a player or target
    bool isPlayerEntity =
        cm.getComponent<components::Player>(entity) != nullptr;
    bool isTargetEntity =
        cm.getComponent<components::Target>(entity) != nullptr;

    if (!isPlayerEntity && !isTargetEntity) {
      continue; // Skip entities that are neither player nor target
    }

    // Check each collision for player-target interaction
    for (const auto &collision : collisionResult->getCollisions()) {
      // Handle the optional Entity properly
      if (!collision.otherEntity.has_value()) {
        continue; // Skip if no other entity
      }
      Entity otherEntity = collision.otherEntity.value();

      // Check if this is a player-target collision (from either perspective)
      bool playerHitTarget =
          isPlayerEntity &&
          cm.getComponent<components::Target>(otherEntity) != nullptr;
      bool targetHitPlayer =
          isTargetEntity &&
          cm.getComponent<components::Player>(otherEntity) != nullptr;

      if (playerHitTarget || targetHitPlayer) {
        state_ = GameState::GAME_OVER;
        // Update ShootingGalleryState to GAME_OVER
        components::ShootingGalleryState &galleryState =
            components::ShootingGalleryState::getInstance();
        galleryState.setState(components::GameState::GAME_OVER);
        SDL_LogInfo(
            SDL_LOG_CATEGORY_APPLICATION,
            "[GameStateSystem] Game Over! Player-Target collision detected! "
            "Entity %llu (%s) collided with Entity %llu (%s)",
            entity.getId(), isPlayerEntity ? "player" : "target",
            otherEntity.getId(), playerHitTarget ? "target" : "player");
        return; // Exit early once game over is triggered
      }
    }
  }
}

bool GameStateSystem::isGameOver() const {
  return state_ == GameState::GAME_OVER;
}

void GameStateSystem::reset() {
  state_ = GameState::RUNNING;
  SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
              "[GameStateSystem] Game state reset to RUNNING");
}

GameState GameStateSystem::getState() const { return state_; }

} // namespace systems
} // namespace ecs
} // namespace game