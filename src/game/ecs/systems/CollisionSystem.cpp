#include "CollisionSystem.hpp"
#include "../ComponentManager.hpp"
#include "../Entity.hpp"
#include "../SystemManager.hpp"
#include "../components/Collision.hpp"
#include "../components/CollisionResult.hpp"
#include "../components/Player.hpp"
#include "../components/Projectile.hpp"
#include "../components/Sprite.hpp"
#include "../components/Target.hpp"
#include "../components/Transform.hpp"
#include <cmath>
#include <vector>

namespace game {
namespace ecs {
namespace systems {

CollisionSystem::CollisionSystem()
    : System(), componentManager_(ComponentManager::getInstance()),
      systemManager_(SystemManager::getInstance()) {

  // Register required components
  registerRequiredComponent<components::Transform>();
  registerRequiredComponent<components::Sprite>();
  registerRequiredComponent<components::Collision>();

  SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
              "[CollisionSystem] Initialized with pure ECS architecture "
              "(component-only)");
}

void CollisionSystem::update(float deltaTime) {
  const auto &entitySet = getEntities();
  std::vector<Entity> entities(entitySet.begin(), entitySet.end());

  // Clear all previous collision results for fresh detection cycle
  clearCollisionResults();

  // Check each pair of entities for collision
  for (size_t i = 0; i < entities.size(); i++) {
    for (size_t j = i + 1; j < entities.size(); j++) {
      const Entity &entityA = entities[i];
      const Entity &entityB = entities[j];

      if (checkCollision(entityA, entityB)) {
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
                    "[CollisionSystem] Collision detected between entity %llu "
                    "and entity %llu",
                    entityA.getId(), entityB.getId());

        // Store collision results in CollisionResult components (pure ECS)
        storeCollisionResult(entityA, entityB);
      }
    }
  }
}

void CollisionSystem::clearCollisionResults() {
  // Clear collision results for all entities that have CollisionResult
  // components
  const auto &allEntities = getEntities();
  int clearedEntities = 0;

  for (const Entity &entity : allEntities) {
    auto *collisionResult =
        componentManager_.getComponent<components::CollisionResult>(entity);
    if (collisionResult != nullptr && collisionResult->isEnabled()) {
      collisionResult->clearCollisions();
      clearedEntities++;
    }
  }

  SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION,
               "[CollisionSystem] Cleared collision results for %d entities",
               clearedEntities);
}

void CollisionSystem::storeCollisionResult(const Entity &entityA,
                                           const Entity &entityB) {
  // Enhanced debug logging
  bool aIsPlayer =
      componentManager_.getComponent<components::Player>(entityA) != nullptr;
  bool aIsTarget =
      componentManager_.getComponent<components::Target>(entityA) != nullptr;
  bool bIsPlayer =
      componentManager_.getComponent<components::Player>(entityB) != nullptr;
  bool bIsTarget =
      componentManager_.getComponent<components::Target>(entityB) != nullptr;

  // Highlight player-target collisions with ERROR level so they stand out
  if ((aIsPlayer && bIsTarget) || (aIsTarget && bIsPlayer)) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                 "*** PLAYER-TARGET COLLISION DETECTED! ***");
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                 "  Entity %llu (Player=%d, Target=%d) <-> Entity %llu "
                 "(Player=%d, Target=%d)",
                 entityA.getId(), aIsPlayer, aIsTarget, entityB.getId(),
                 bIsPlayer, bIsTarget);
  } else {
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
                "[CollisionSystem] Storing collision: Entity %llu (Player=%d, "
                "Target=%d) <-> Entity %llu (Player=%d, Target=%d)",
                entityA.getId(), aIsPlayer, aIsTarget, entityB.getId(),
                bIsPlayer, bIsTarget);
  }

  // Calculate collision point and normal
  CollisionInfo info = calculateCollisionInfo(entityA, entityB);

  // Ensure both entities have CollisionResult components
  ensureCollisionResultComponent(entityA);
  ensureCollisionResultComponent(entityB);

  // Store collision in entity A
  auto *collisionResultA =
      componentManager_.getComponent<components::CollisionResult>(entityA);
  if (collisionResultA != nullptr && collisionResultA->isEnabled()) {
    collisionResultA->addCollision(entityA, entityB, info.collisionPoint,
                                   info.collisionNormal);
    SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION,
                 "[CollisionSystem] Stored collision result for entity %llu",
                 entityA.getId());
  }

  // Store collision in entity B
  auto *collisionResultB =
      componentManager_.getComponent<components::CollisionResult>(entityB);
  if (collisionResultB != nullptr && collisionResultB->isEnabled()) {
    collisionResultB->addCollision(entityA, entityB, info.collisionPoint,
                                   info.collisionNormal);
    SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION,
                 "[CollisionSystem] Stored collision result for entity %llu",
                 entityB.getId());
  }
}

void CollisionSystem::ensureCollisionResultComponent(const Entity &entity) {
  auto *collisionResult =
      componentManager_.getComponent<components::CollisionResult>(entity);
  if (collisionResult == nullptr) {
    // Create and add CollisionResult component
    componentManager_.addComponent<components::CollisionResult>(entity);
    systemManager_.onComponentAdded(
        entity, std::type_index(typeid(components::CollisionResult)));
    SDL_LogDebug(
        SDL_LOG_CATEGORY_APPLICATION,
        "[CollisionSystem] Created CollisionResult component for entity %llu",
        entity.getId());
  }
}

bool CollisionSystem::checkCollision(const Entity &entityA,
                                     const Entity &entityB) {
  auto *transformA =
      componentManager_.getComponent<components::Transform>(entityA);
  auto *spriteA = componentManager_.getComponent<components::Sprite>(entityA);
  auto *transformB =
      componentManager_.getComponent<components::Transform>(entityB);
  auto *spriteB = componentManager_.getComponent<components::Sprite>(entityB);

  if (!transformA || !spriteA || !transformB || !spriteB) {
    return false;
  }

  // AABB collision check
  return (transformA->getPosition().x <
              transformB->getPosition().x + spriteB->getWidth() &&
          transformA->getPosition().x + spriteA->getWidth() >
              transformB->getPosition().x &&
          transformA->getPosition().y <
              transformB->getPosition().y + spriteB->getHeight() &&
          transformA->getPosition().y + spriteA->getHeight() >
              transformB->getPosition().y);
}

CollisionSystem::CollisionInfo
CollisionSystem::calculateCollisionInfo(const Entity &entityA,
                                        const Entity &entityB) {
  auto *transformA =
      componentManager_.getComponent<components::Transform>(entityA);
  auto *spriteA = componentManager_.getComponent<components::Sprite>(entityA);
  auto *transformB =
      componentManager_.getComponent<components::Transform>(entityB);
  auto *spriteB = componentManager_.getComponent<components::Sprite>(entityB);

  // Calculate centers of both entities
  Vector2 centerA(transformA->getPosition().x + spriteA->getWidth() / 2.0f,
                  transformA->getPosition().y + spriteA->getHeight() / 2.0f);
  Vector2 centerB(transformB->getPosition().x + spriteB->getWidth() / 2.0f,
                  transformB->getPosition().y + spriteB->getHeight() / 2.0f);

  // Calculate collision point (midpoint between centers)
  Vector2 collisionPoint((centerA.x + centerB.x) / 2.0f,
                         (centerA.y + centerB.y) / 2.0f);

  // Calculate collision normal (normalized vector from centerA to centerB)
  float dx = centerB.x - centerA.x;
  float dy = centerB.y - centerA.y;
  float length = std::sqrt(dx * dx + dy * dy);

  Vector2 collisionNormal;
  if (length == 0.0f) {
    // If centers are at same point, use default normal
    collisionNormal = Vector2(1.0f, 0.0f);
  } else {
    collisionNormal = Vector2(dx / length, dy / length);
  }

  return CollisionInfo(collisionPoint, collisionNormal);
}

std::string CollisionSystem::toString() const {
  return "CollisionSystem(entities=" + std::to_string(getEntities().size()) +
         ", pure-ECS)";
}

} // namespace systems
} // namespace ecs
} // namespace game