#include "DuckMovementSystem.hpp"
#include "../ComponentManager.hpp"
#include "../components/Expirable.hpp"
#include "../components/Images.hpp"
#include "../components/Movement.hpp"
#include "../components/Player.hpp"
#include "../components/Target.hpp"
#include "../components/Transform.hpp"
#include <cmath>
#include <sstream>

namespace game {
namespace ecs {
namespace systems {

DuckMovementSystem::DuckMovementSystem(float worldWidth, float worldHeight)
    : System(), worldWidth_(worldWidth), worldHeight_(worldHeight) {

  // Register required components
  registerRequiredComponent<components::Transform>();
  registerRequiredComponent<components::Movement>();
  registerRequiredComponent<components::Target>();
  registerRequiredComponent<components::Expirable>();

  // Register optional component for sprite direction
  registerOptionalComponent<components::Images>();

  SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
              "[DuckMovementSystem] Initialized with world %fx%f", worldWidth_,
              worldHeight_);
}

void DuckMovementSystem::update(float deltaTime) {
  ComponentManager &cm = ComponentManager::getInstance();

  // Find the player entity
  Entity playerEntity;
  bool foundPlayer = false;
  for (const auto &entity : getEntities()) {
    if (cm.getComponent<components::Player>(entity)) {
      playerEntity = entity;
      foundPlayer = true;
      break;
    }
  }

  if (!foundPlayer) {
    SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION,
                "[DuckMovementSystem] No player found for pawn tracking");
    return;
  }

  auto *playerTransform = cm.getComponent<components::Transform>(playerEntity);
  if (!playerTransform) {
    SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION,
                "[DuckMovementSystem] Player has no Transform component");
    return;
  }

  for (const Entity &entity : getEntities()) {
    auto *transform = cm.getComponent<components::Transform>(entity);
    auto *movement = cm.getComponent<components::Movement>(entity);
    auto *target = cm.getComponent<components::Target>(entity);
    auto *expirable = cm.getComponent<components::Expirable>(entity);

    if (!transform || !movement || !target || !expirable) {
      continue;
    }

    if (!movement->isEnabled() || expirable->isExpired()) {
      continue;
    }

    // Calculate direction to player
    Vector2 toPlayer =
        playerTransform->getPosition() - transform->getPosition();
    float distance =
        std::sqrt(toPlayer.x * toPlayer.x + toPlayer.y * toPlayer.y);

    if (distance > 0.0f) {
      // Normalize direction
      toPlayer.x /= distance;
      toPlayer.y /= distance;

      // Set velocity towards player
      float speed = 200.0f; // Default speed
      if (target->getTargetType() == "regular") {
        speed = 200.0f;
      } else if (target->getTargetType() == "boss") {
        speed = 100.0f;
      }

      movement->setVelocity(Vector2(toPlayer.x * speed, toPlayer.y * speed));

      // Calculate rotation angle
      float angle = std::atan2(toPlayer.y, toPlayer.x) * 180.0f / M_PI;
      transform->setRotation(angle);
    }

    // Update position based on velocity
    float newX =
        transform->getPosition().x + movement->getVelocity().x * deltaTime;
    float newY =
        transform->getPosition().y + movement->getVelocity().y * deltaTime;
    transform->setPosition(Vector2(newX, newY));

    // Check if pawn has gone off screen
    if (newX < -50.0f || newX > worldWidth_ + 50.0f || newY < -50.0f ||
        newY > worldHeight_ + 50.0f) {
      expirable->markExpired();
    }
  }
}

std::string DuckMovementSystem::toString() const {
  return "DuckMovementSystem(entities=" + std::to_string(getEntities().size()) +
         ")";
}

} // namespace systems
} // namespace ecs
} // namespace game