#include "PlayerControlSystem.hpp"
#include "../SystemManager.hpp"
#include "../components/ShootRequest.hpp"
#include <SDL3/SDL.h>
#include <algorithm>

namespace game::ecs::systems {

PlayerControlSystem::PlayerControlSystem(
    game::events::EventManager &eventManager, float worldWidth,
    float worldHeight)
    : System(), eventManager_(eventManager), worldWidth_(worldWidth),
      worldHeight_(worldHeight) {
  // Register required components (matches Python/Java)
  registerRequiredComponent<game::ecs::components::Transform>();
  registerRequiredComponent<game::ecs::components::Player>();
  registerRequiredComponent<game::ecs::components::Input>();

  // Register KeyboardInput as optional component for dual input support
  registerOptionalComponent<game::ecs::components::KeyboardInput>();

  // Subscribe to keyboard events (matches Python/Java pattern)
  // Note: EventManager subscription will be handled by GameWorld

  SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
              "PlayerControlSystem initialized with dual input support, world "
              "size: %.1fx%.1f",
              worldWidth_, worldHeight_);
}

void PlayerControlSystem::onEvent(const game::events::Event &event) {
  // Check if this is a keyboard event
  const auto *keyboardEvent =
      dynamic_cast<const game::events::KeyboardEvent *>(&event);
  if (!keyboardEvent) {
    // Not a keyboard event, ignore
    return;
  }

  // Convert key to lowercase string (matches Python/Java)
  std::string keyString = keyboardEvent->getKeyText();
  std::transform(keyString.begin(), keyString.end(), keyString.begin(),
                 ::tolower);

  SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
              "PlayerControlSystem received keyboard event: key=%s, pressed=%s",
              keyString.c_str(), keyboardEvent->isPressed() ? "true" : "false");

  // Update pressed keys state (matches Python/Java)
  if (keyboardEvent->isPressed()) {
    pressedKeys_.insert(keyString);
  } else {
    pressedKeys_.erase(keyString);
  }

  SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
              "PlayerControlSystem pressed_keys after update: {%s}",
              [this]() {
                std::string keys;
                for (const auto &key : pressedKeys_) {
                  if (!keys.empty())
                    keys += ", ";
                  keys += key;
                }
                return keys;
              }()
                  .c_str());
}

void PlayerControlSystem::update(float deltaTime) {
  auto &gameState = game::ecs::components::ShootingGalleryState::getInstance();

  SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
              "PlayerControlSystem update: game_state=%s, entities=%zu, "
              "pressed_keys_count=%zu",
              gameState.getStateString().c_str(), getEntities().size(),
              pressedKeys_.size());

  // Only process input during gameplay (matches Python/Java)
  if (!gameState.isPlaying()) {
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
                "PlayerControlSystem: Game not playing, state is %s",
                gameState.getStateString().c_str());
    return;
  }

  for (const auto &entity : getEntities()) {
    // Check if entity has all required components
    if (!hasRequiredComponents(entity)) {
      SDL_LogInfo(
          SDL_LOG_CATEGORY_APPLICATION,
          "PlayerControlSystem: Entity %llu missing required components",
          entity.getId());
      continue;
    }

    auto *transform =
        getComponentManager()->getComponent<game::ecs::components::Transform>(
            entity);
    auto *player =
        getComponentManager()->getComponent<game::ecs::components::Player>(
            entity);
    auto *input =
        getComponentManager()->getComponent<game::ecs::components::Input>(
            entity);

    // Get optional KeyboardInput component for dual input support
    auto *keyboardInput =
        getOptionalComponent<game::ecs::components::KeyboardInput>(entity);

    if (!input->isEnabled()) {
      SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
                  "PlayerControlSystem: Entity %llu input disabled",
                  entity.getId());
      continue;
    }

    // Log input method being used
    if (keyboardInput && keyboardInput->isEnabled()) {
      SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
                  "PlayerControlSystem: Entity %llu using KeyboardInput "
                  "component (primary)",
                  entity.getId());
    } else {
      SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
                  "PlayerControlSystem: Entity %llu using event-driven input "
                  "(fallback)",
                  entity.getId());
    }

    SDL_LogInfo(
        SDL_LOG_CATEGORY_APPLICATION,
        "PlayerControlSystem: Processing entity %llu at pos (%.1f, %.1f)",
        entity.getId(), transform->getPosition().x, transform->getPosition().y);

    // Handle movement input with dual input support (matches Python/Java)
    handleMovement(entity, transform, input, keyboardInput, deltaTime);

    // Handle shooting input with dual input support (matches Python/Java)
    handleShooting(entity, transform, player, input, keyboardInput);
  }
}

void PlayerControlSystem::handleMovement(
    const Entity &entity, game::ecs::components::Transform *transform,
    game::ecs::components::Input *input,
    game::ecs::components::KeyboardInput *keyboardInput, float deltaTime) {
  // Get movement keys with defaults (matches Python/Java logic)
  std::string leftKey = input->getKey("left");
  if (leftKey.empty()) {
    leftKey = "ArrowLeft"; // Default left key
  }

  std::string rightKey = input->getKey("right");
  if (rightKey.empty()) {
    rightKey = "ArrowRight"; // Default right key
  }

  std::string upKey = input->getKey("up");
  if (upKey.empty()) {
    upKey = "ArrowUp"; // Default right key
  }

  std::string downKey = input->getKey("down");
  if (downKey.empty()) {
    downKey = "ArrowDown"; // Default right key
  }

  SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
              "PlayerControlSystem: Using keys - left='%s', right='%s', "
              "up='%s', down='%s'",
              leftKey.c_str(), rightKey.c_str(), upKey.c_str(),
              downKey.c_str());

  // Calculate movement using dual input approach (matches Python/Java)
  float movementSpeed = input->getMoveSpeed() * deltaTime;
  float dx = 0.0f;
  float dy = 0.0f;
  float targetRotation = transform->getRotation();

  // Check left key using dual input approach
  bool leftPressed = isKeyPressed(leftKey, keyboardInput) ||
                     isKeyPressed("a", keyboardInput) ||
                     isKeyPressed("arrowleft", keyboardInput);

  // Check right key using dual input approach
  bool rightPressed = isKeyPressed(rightKey, keyboardInput) ||
                      isKeyPressed("d", keyboardInput) ||
                      isKeyPressed("arrowright", keyboardInput);

  // Check up key using dual input approach
  bool upPressed = isKeyPressed(upKey, keyboardInput) ||
                   isKeyPressed("w", keyboardInput) ||
                   isKeyPressed("arrowup", keyboardInput);

  // Check down key using dual input approach
  bool downPressed = isKeyPressed(downKey, keyboardInput) ||
                     isKeyPressed("s", keyboardInput) ||
                     isKeyPressed("arrowdown", keyboardInput);

  if (leftPressed) {
    dx -= movementSpeed;
    targetRotation = 180.0f; // Facing left
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
                "PlayerControlSystem: LEFT movement detected, dx=%.2f", dx);
  }
  if (rightPressed) {
    dx += movementSpeed;
    targetRotation = 0.0f; // Facing right
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
                "PlayerControlSystem: RIGHT movement detected, dx=%.2f", dx);
  }
  if (upPressed) {
    dy -= movementSpeed;
    targetRotation = 270.0f; // Facing up
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
                "PlayerControlSystem: UP movement detected, dy=%.2f", dy);
  }
  if (downPressed) {
    dy += movementSpeed;
    targetRotation = 90.0f; // Facing down
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
                "PlayerControlSystem: DOWN movement detected, dy=%.2f", dy);
  }

  // Add diagonal movement handling
  if (upPressed && rightPressed) {
    targetRotation = 315.0f; // Facing up-right
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
                "PlayerControlSystem: UP-RIGHT movement detected");
  } else if (upPressed && leftPressed) {
    targetRotation = 225.0f; // Facing up-left
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
                "PlayerControlSystem: UP-LEFT movement detected");
  } else if (downPressed && rightPressed) {
    targetRotation = 45.0f; // Facing down-right
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
                "PlayerControlSystem: DOWN-RIGHT movement detected");
  } else if (downPressed && leftPressed) {
    targetRotation = 135.0f; // Facing down-left
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
                "PlayerControlSystem: DOWN-LEFT movement detected");
  }

  // Normalize diagonal movement speed
  if ((upPressed || downPressed) && (leftPressed || rightPressed)) {
    // Normalize diagonal movement to prevent faster diagonal movement
    float length = std::sqrt(dx * dx + dy * dy);
    if (length > 0) {
      dx = (dx / length) * movementSpeed;
      dy = (dy / length) * movementSpeed;
    }
  }

  float currentRotation = transform->getRotation();
  float rotationSpeed = 720.0f * deltaTime; // Degrees per second
  float newRotation = currentRotation;

  // Calculate the shortest path to the target rotation
  float diff = targetRotation - currentRotation;
  if (diff > 180.0f)
    diff -= 360.0f;
  if (diff < -180.0f)
    diff += 360.0f;

  // Apply rotation with smoothing
  if (std::abs(diff) > 0.1f) {
    newRotation += std::copysign(std::min(std::abs(diff), rotationSpeed), diff);
    // Normalize rotation to 0-360 range
    while (newRotation >= 360.0f)
      newRotation -= 360.0f;
    while (newRotation < 0.0f)
      newRotation += 360.0f;
  }

  if (dx != 0.0f || dy != 0.0f) {
    // Apply movement with boundary checking (matches Python/Java)
    float newX = transform->getPosition().x + dx;
    float newY = transform->getPosition().y + dy;

    // Get sprite width for boundary checking (matches Python/Java)
    auto *sprite =
        getComponentManager()->getComponent<game::ecs::components::Sprite>(
            entity);
    if (sprite) {
      float halfWidth = sprite->getWidth() / 2.0f;
      float halfHeight = sprite->getHeight() / 2.0f;
      newX = std::max(halfWidth, std::min(worldWidth_ - halfWidth, newX));
      newY = std::max(halfHeight, std::min(worldHeight_ - halfHeight, newY));
    }

    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
                "PlayerControlSystem: Moving from (%.1f, %.1f) to (%.1f, %.1f) "
                "(dx=%.2f, dy=%.2f)",
                transform->getPosition().x, transform->getPosition().y, newX,
                newY, dx, dy);

    transform->setPosition(newX, newY);
    transform->setRotation(newRotation);
  }
}

void PlayerControlSystem::handleShooting(
    const Entity &entity, game::ecs::components::Transform *transform,
    game::ecs::components::Player *player, game::ecs::components::Input *input,
    game::ecs::components::KeyboardInput *keyboardInput) {
  SDL_LogInfo(
      SDL_LOG_CATEGORY_APPLICATION,
      "PlayerControlSystem.handleShooting: checking shooting for entity %llu",
      entity.getId());

  // Get fire key - check for both "fire" and "space" mappings (matches Java
  // logic)
  std::string fireKey = input->getKey("fire");
  if (fireKey.empty()) {
    fireKey = input->getKey("space");
  }
  if (fireKey.empty()) {
    fireKey = "space"; // Default to logical space key name
  }

  SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
              "PlayerControlSystem: Fire key mapped to: '%s'", fireKey.c_str());

  // Use dual input approach with space key variations (matches Python/Java)
  bool fireKeyPressed = isKeyPressed(fireKey, keyboardInput) ||
                        isKeyPressed(" ", keyboardInput) ||
                        isKeyPressed("space", keyboardInput);
  SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
              "PlayerControlSystem: Fire key '%s' pressed: %s", fireKey.c_str(),
              fireKeyPressed ? "YES" : "NO");

  // Debug: Check Player state
  SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
              "PlayerControlSystem: Player canFire(): %s (fireRate=%.2f)",
              player->canFire() ? "YES" : "NO", player->getFireRate());

  // Check if fire key is pressed and player can fire (matches Python/Java)
  if (fireKeyPressed) {
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
                "🔥 PlayerControlSystem: FIRE key '%s' is pressed!",
                fireKey.c_str());

    if (player->canFire()) {
      SDL_LogInfo(
          SDL_LOG_CATEGORY_APPLICATION,
          "🚀 PlayerControlSystem: Player can fire, creating shoot request!");

      // Calculate center X position of the player sprite (matches Python/Java)
      auto *sprite =
          getComponentManager()->getComponent<game::ecs::components::Sprite>(
              entity);
      float centerX = transform->getPosition().x;
      float centerY = transform->getPosition().y;
      if (sprite) {
        centerX = transform->getPosition().x + (sprite->getWidth() / 2.0f);
        centerY = transform->getPosition().y + (sprite->getHeight() / 2.0f);
      }

      float rotationRadians = transform->getRotation() * M_PI / 180.0f;

      // Claculate the direction vector based on the rotation
      float dirX = cos(rotationRadians);
      float dirY = sin(rotationRadians);

      // Create ShootRequest component instead of directly creating projectile
      createShootRequest(entity, centerX, centerY, dirX, dirY);
      player->fire();

      // Record shot in game state (matches Python/Java)
      auto &gameState =
          game::ecs::components::ShootingGalleryState::getInstance();
      gameState.recordShot();

      SDL_LogInfo(
          SDL_LOG_CATEGORY_APPLICATION,
          "✅ PlayerControlSystem: ShootRequest created and shot recorded!");
    } else {
      SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
                  "⏰ PlayerControlSystem: Player cannot fire yet (cooldown)");
    }
  } else {
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
                "PlayerControlSystem: Fire key '%s' NOT pressed",
                fireKey.c_str());
  }
}

bool PlayerControlSystem::isKeyPressed(
    const std::string &key,
    game::ecs::components::KeyboardInput *keyboardInput) const {
  // Primary: Use KeyboardInput component if available and enabled
  if (keyboardInput && keyboardInput->isEnabled()) {
    return keyboardInput->isPressed(key);
  }

  // Fallback: Use event-driven input (pressedKeys_ set)
  return pressedKeys_.count(key) > 0;
}

void PlayerControlSystem::createShootRequest(const Entity &entity, float x,
                                             float y, float dirX, float dirY) {
  SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
              "PlayerControlSystem.createShootRequest: Creating shoot request "
              "for entity %llu at (%.1f, %.1f) with direction (%.2f, %.2f)",
              entity.getId(), x, y, dirX, dirY);

  try {

    float offset = 10.0f; // Distance from entity center
    float startX = x + (dirX * offset);
    float startY = y + (dirY * offset);

    // Create ShootRequest component with position
    getComponentManager()->addComponent<game::ecs::components::ShootRequest>(
        entity, startX, startY, dirX, dirY);

    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
                "🎯 PlayerControlSystem: SHOOT REQUEST CREATED - Entity %llu at "
                "(%.1f, %.1f) with direction (%.2f, %.2f)",
                entity.getId(), startX, startY, dirX, dirY);

  } catch (const std::exception &e) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                 "PlayerControlSystem: ERROR creating shoot request: %s",
                 e.what());
  }
}

void PlayerControlSystem::createProjectile(float x, float y) {
  SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
              "PlayerControlSystem.createProjectile: Starting projectile "
              "creation at (%.1f, %.1f)",
              x, y);

  try {
    // Create projectile entity (using C++ Entity::create pattern)
    Entity projectileEntity = Entity::create("projectile");
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
                "PlayerControlSystem: Created projectile entity: %llu",
                projectileEntity.getId());

    // Calculate missile starting position (matches Java)
    float missileStartY = y - 10.0f; // Spawn slightly above player

    // Add Transform component (matches Java)
    game::ecs::Vector2 position(x, missileStartY);
    getComponentManager()->addComponent<game::ecs::components::Transform>(
        projectileEntity, position);
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
                "PlayerControlSystem: Added Transform at (%.1f, %.1f)", x,
                missileStartY);

    // Add Sprite component (matches Java - 4x10 yellow projectile)
    SDL_Color yellowColor = {255, 255, 0, 255};
    getComponentManager()->addComponent<game::ecs::components::Sprite>(
        projectileEntity, 4.0f, 10.0f, yellowColor);
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
                "PlayerControlSystem: Added Sprite component (4x10, yellow)");

    // Add Projectile component first to get speed and max range (matches Java)
    float maxRange =
        missileStartY -
        10.0f; // Distance from missile start to screen top minus buffer
    getComponentManager()->addComponent<game::ecs::components::Projectile>(
        projectileEntity, 400.0f, maxRange);
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
                "PlayerControlSystem: Added Projectile component (speed=400, "
                "range=%.1f)",
                maxRange);

    // Add Movement component (upward velocity) (matches Java)
    game::ecs::Vector2 velocity(0.0f, -400.0f);
    getComponentManager()->addComponent<game::ecs::components::Movement>(
        projectileEntity, velocity);
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
                "PlayerControlSystem: Added Movement with velocity (0, -400)");

    // Add Expirable component (matches Java)
    getComponentManager()->addComponent<game::ecs::components::Expirable>(
        projectileEntity);
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
                "PlayerControlSystem: Added Expirable component");

    // Add Collision component for projectile (matches Java)
    getComponentManager()->addComponent<game::ecs::components::Collision>(
        projectileEntity);
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
                "PlayerControlSystem: Added Collision component");

    // Notify SystemManager about new entity (matches Java)
    game::ecs::SystemManager::getInstance().onEntityCreated(projectileEntity);
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
                "PlayerControlSystem: Notified SystemManager of new entity");

    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
                "🎯 PlayerControlSystem: PROJECTILE CREATED - Entity %llu with "
                "range %.1f",
                projectileEntity.getId(), maxRange);

  } catch (const std::exception &e) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                 "PlayerControlSystem: ERROR creating projectile: %s",
                 e.what());
  }
}

void PlayerControlSystem::onEntityAdded(const Entity &entity) {
  SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
              "PlayerControlSystem: Entity %llu added to system",
              entity.getId());
}

void PlayerControlSystem::onEntityRemoved(const Entity &entity) {
  SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
              "PlayerControlSystem: Entity %llu removed from system",
              entity.getId());
}

} // namespace game::ecs::systems