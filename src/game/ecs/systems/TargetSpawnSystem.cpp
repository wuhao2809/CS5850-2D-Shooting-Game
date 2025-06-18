#include "TargetSpawnSystem.hpp"
#include "../ComponentManager.hpp"
#include "../SystemManager.hpp"
#include "../components/Collision.hpp"
#include "../components/Expirable.hpp"
#include "../components/Images.hpp"
#include "../components/Movement.hpp"
#include "../components/Sprite.hpp"
#include "../components/Target.hpp"
#include "../components/Transform.hpp"
#include <sstream>

namespace game {
namespace ecs {
namespace systems {

TargetSpawnSystem::TargetSpawnSystem(float worldWidth, float worldHeight)
    : System(), worldWidth_(worldWidth), worldHeight_(worldHeight),
      spawnAreaBottom_(worldHeight * 0.6f) // 60% from top
      ,
      randomEngine_(randomDevice_()), distribution_(0.0f, 1.0f) {

  // Target type probabilities (should sum to 1.0)
  targetWeights_["boss"] = 0.1f;   // 10% chance
  targetWeights_["regular"] = 0.9; // 100% chance

  SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
              "[TargetSpawnSystem] Initialized with world %fx%f", worldWidth_,
              worldHeight_);
}

void TargetSpawnSystem::setTemplates(
    const std::unordered_map<std::string, nlohmann::json> &templates) {
  templates_ = templates;
  SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
              "[TargetSpawnSystem] Loaded %zu templates", templates_.size());

  // Log available template names
  std::stringstream ss;
  for (const auto &[name, _] : templates_) {
    ss << name << " ";
  }
  SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
              "[TargetSpawnSystem] Available templates: %s", ss.str().c_str());
}

void TargetSpawnSystem::update(float deltaTime) {
  components::ShootingGalleryState &gameState =
      components::ShootingGalleryState::getInstance();

  // Only spawn targets during gameplay
  if (!gameState.isPlaying()) {
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
                "[TargetSpawnSystem] Game not playing, state is %s",
                gameState.getStateString().c_str());
    return;
  }

  // Check if there are any active ducks on screen
  int activeDucks = countActiveDucks();
  SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
              "[TargetSpawnSystem] update: active_ducks=%d", activeDucks);

  // Check if it's time to spawn a new target
  bool shouldSpawn = gameState.shouldSpawnTarget();
  SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
              "[TargetSpawnSystem] should_spawn=%s",
              shouldSpawn ? "true" : "false");

  if (shouldSpawn) {
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
                "[TargetSpawnSystem] Spawning new duck!");
    spawnTarget();
  } else {
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
                "[TargetSpawnSystem] Not time to spawn duck yet");
  }
}

int TargetSpawnSystem::countActiveDucks() const {
  int count = 0;
  ComponentManager &cm = ComponentManager::getInstance();

  // Check all entities that this system manages
  for (const Entity &entity : getEntities()) {
    // Check if entity has Target and Expirable components
    auto *target = cm.getComponent<components::Target>(entity);
    auto *expirable = cm.getComponent<components::Expirable>(entity);

    if (target && expirable && target->getTargetType() == "duck" &&
        !expirable->isExpired()) {
      count++;
    }
  }

  return count;
}

void TargetSpawnSystem::spawnTarget() {
  // Choose target type based on weights
  std::string targetType = chooseTargetType();

  // Choose which edge to spawn from (0: top, 1: right, 2: bottom, 3: left)
  int edge = static_cast<int>(distribution_(randomEngine_) * 4);

  float x, y;
  Vector2 direction;

  switch (edge) {
  case 0: // Top edge
    x = distribution_(randomEngine_) * worldWidth_;
    y = -25.0f;
    direction = Vector2(0.0f, 1.0f);
    break;
  case 1: // Right edge
    x = worldWidth_ + 25.0f;
    y = distribution_(randomEngine_) * worldHeight_;
    direction = Vector2(-1.0f, 0.0f);
    break;
  case 2: // Bottom edge
    x = distribution_(randomEngine_) * worldWidth_;
    y = worldHeight_ + 25.0f;
    direction = Vector2(0.0f, -1.0f);
    break;
  case 3: // Left edge
    x = -25.0f;
    y = distribution_(randomEngine_) * worldHeight_;
    direction = Vector2(1.0f, 0.0f);
    break;
  }

  // Get template name based on target type
  std::string templateName =
      (targetType == "regular") ? "duck_regular" : "duck_boss";

  // Create target entity
  Entity targetEntity =
      Entity::create("pawn_" + std::to_string(SDL_GetTicks()));

  // Notify SystemManager about new entity BEFORE adding components
  auto &systemManager = SystemManager::getInstance();
  systemManager.onEntityCreated(targetEntity);

  // Configure target using template
  createDuckFromTemplate(targetEntity, x, y, direction, templateName);

  // Add entity to this system for tracking
  addEntity(targetEntity);

  // Get the point value from the Target component for logging
  ComponentManager &cm = ComponentManager::getInstance();
  auto *targetComponent = cm.getComponent<components::Target>(targetEntity);
  int pointValue = targetComponent ? targetComponent->getPointValue() : 0;

  SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
              "[TargetSpawnSystem] Created %s pawn at (%.1f, %.1f) from edge "
              "%d, worth %d points",
              targetType.c_str(), x, y, edge, pointValue);
}

std::string TargetSpawnSystem::chooseTargetType() {
  float randValue = distribution_(randomEngine_);
  float cumulativeWeight = 0.0f;

  for (const auto &[targetType, weight] : targetWeights_) {
    cumulativeWeight += weight;
    if (randValue <= cumulativeWeight) {
      return targetType;
    }
  }

  // Fallback to regular if something goes wrong
  return "regular";
}

float TargetSpawnSystem::getFlightLevelFromTemplate(
    const std::string &templateName) const {
  auto it = templates_.find(templateName);
  if (it == templates_.end()) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                 "[TargetSpawnSystem] Template '%s' not found. Using default "
                 "flight level.",
                 templateName.c_str());
    return 300.0f; // Default to lower row
  }

  const auto &template_json = it->second;
  if (template_json.contains("components") &&
      template_json["components"].contains("flightLevel") &&
      template_json["components"]["flightLevel"].contains("y")) {
    return template_json["components"]["flightLevel"]["y"].get<float>();
  }

  SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION,
              "[TargetSpawnSystem] No flightLevel component in template '%s'. "
              "Using default.",
              templateName.c_str());
  return 300.0f; // Default flight level
}

float TargetSpawnSystem::getSpeedFromTemplate(
    const std::string &templateName) const {
  auto it = templates_.find(templateName);
  if (it == templates_.end()) {
    SDL_LogError(
        SDL_LOG_CATEGORY_APPLICATION,
        "[TargetSpawnSystem] Template '%s' not found. Using default speed.",
        templateName.c_str());
    return 200.0f; // Default speed
  }

  const auto &template_json = it->second;
  if (template_json.contains("components") &&
      template_json["components"].contains("speed") &&
      template_json["components"]["speed"].contains("value")) {
    return template_json["components"]["speed"]["value"].get<float>();
  }

  SDL_LogWarn(
      SDL_LOG_CATEGORY_APPLICATION,
      "[TargetSpawnSystem] No speed component in template '%s'. Using default.",
      templateName.c_str());
  return 200.0f; // Default speed
}

void TargetSpawnSystem::createDuckFromTemplate(
    const Entity &entity, float x, float y, const Vector2 &direction,
    const std::string &templateName) {
  auto it = templates_.find(templateName);
  if (it == templates_.end()) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                 "[TargetSpawnSystem] Template '%s' not found.",
                 templateName.c_str());
    return;
  }

  const auto &template_json = it->second;
  if (!template_json.contains("components")) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                 "[TargetSpawnSystem] Template '%s' has no components.",
                 templateName.c_str());
    return;
  }

  const auto &components = template_json["components"];
  ComponentManager &cm = ComponentManager::getInstance();
  auto &systemManager = SystemManager::getInstance();

  // Add Transform component with initial rotation based on direction
  float initialRotation = std::atan2(direction.y, direction.x) * 180.0f / M_PI;
  cm.addComponent<components::Transform>(entity, Vector2(x, y), initialRotation,
                                         Vector2(1.0f, 1.0f));
  systemManager.onComponentAdded(
      entity, std::type_index(typeid(components::Transform)));

  // Add Sprite component from template
  if (components.contains("sprite")) {
    const auto &spriteData = components["sprite"];
    const auto &colorData = spriteData["color"];

    SDL_Color color = {
        static_cast<uint8_t>(colorData["r"].get<int>()),
        static_cast<uint8_t>(colorData["g"].get<int>()),
        static_cast<uint8_t>(colorData["b"].get<int>()),
        255 // Alpha
    };

    cm.addComponent<components::Sprite>(
        entity, spriteData["width"].get<float>(),
        spriteData["height"].get<float>(), color);
    systemManager.onComponentAdded(entity,
                                   std::type_index(typeid(components::Sprite)));
  }

  // Add Images component from template
  if (components.contains("images")) {
    const auto &imagesData = components["images"];
    cm.addComponent<components::Images>(entity);

    // Get the Images component to configure it
    auto *images = cm.getComponent<components::Images>(entity);
    if (images && imagesData.contains("imageNames")) {
      for (const auto &imageName : imagesData["imageNames"]) {
        images->addImage(imageName.get<std::string>());
      }
      // Always use the first image since we're rotating the sprite
      images->setCurrentImage(0);
    }
    systemManager.onComponentAdded(entity,
                                   std::type_index(typeid(components::Images)));
  }

  // Add Movement component with velocity based on direction
  float speed = getSpeedFromTemplate(templateName);
  cm.addComponent<components::Movement>(
      entity, Vector2(direction.x * speed, direction.y * speed));
  systemManager.onComponentAdded(entity,
                                 std::type_index(typeid(components::Movement)));

  // Add Target component from template
  if (components.contains("target")) {
    const auto &targetData = components["target"];
    int pointValue = targetData.contains("pointValue")
                         ? targetData["pointValue"].get<int>()
                         : 10; // Default point value
    std::string targetType = targetData.contains("targetType")
                                 ? targetData["targetType"].get<std::string>()
                                 : "regular";
    cm.addComponent<components::Target>(entity, pointValue, targetType);
    systemManager.onComponentAdded(entity,
                                   std::type_index(typeid(components::Target)));
  }

  // Add Collision component
  cm.addComponent<components::Collision>(entity);
  systemManager.onComponentAdded(
      entity, std::type_index(typeid(components::Collision)));

  // Add Expirable component
  cm.addComponent<components::Expirable>(entity);
  systemManager.onComponentAdded(
      entity, std::type_index(typeid(components::Expirable)));
}

std::string TargetSpawnSystem::toString() const {
  return "TargetSpawnSystem(world=" + std::to_string(worldWidth_) + "x" +
         std::to_string(worldHeight_) + ")";
}

} // namespace systems
} // namespace ecs
} // namespace game