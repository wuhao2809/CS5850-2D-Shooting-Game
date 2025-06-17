#include "RenderSystem.hpp"
#include "../../resources/ResourceManager.hpp"
#include "../ComponentManager.hpp"
#include "../Entity.hpp"
#include "../components/Images.hpp"
#include "../components/Sprite.hpp"
#include "../components/Transform.hpp"
#include <sstream>

namespace game {
namespace ecs {
namespace systems {

RenderSystem::RenderSystem(SDL_Renderer *renderer,
                           const SDL_Color &backgroundColor)
    : System(), renderer_(renderer), backgroundColor_(backgroundColor) {

  // Register required components
  registerRequiredComponent<components::Transform>();
  registerRequiredComponent<components::Sprite>();

  // Register optional components
  registerOptionalComponent<components::Images>();

  SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
              "[RenderSystem] Initialized with required components: Transform "
              "and Sprite");
  SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
              "[RenderSystem] Initialized with optional component: Images");
}

void RenderSystem::setRenderer(SDL_Renderer *renderer) {
  renderer_ = renderer;
  SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
              "[RenderSystem] Renderer set to: %p", renderer);
}

void RenderSystem::update(float deltaTime) {
  if (!renderer_) {
    SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION,
                "[RenderSystem] No renderer set for rendering");
    return;
  }

  // Clear the screen with the background color
  SDL_SetRenderDrawColor(renderer_, backgroundColor_.r, backgroundColor_.g,
                         backgroundColor_.b, backgroundColor_.a);
  SDL_RenderClear(renderer_);

  const auto &entities = getEntities();
  SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
              "[RenderSystem] Update called with %zu entities",
              entities.size());

  ComponentManager &cm = ComponentManager::getInstance();

  for (const Entity &entity : entities) {
    auto *transform = cm.getComponent<components::Transform>(entity);
    auto *sprite = cm.getComponent<components::Sprite>(entity);
    auto *images = getOptionalComponent<components::Images>(entity);

    if (!transform || !sprite) {
      SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION,
                  "[RenderSystem] Entity %llu missing required components",
                  entity.getId());
      continue;
    }

    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
                "[RenderSystem] Rendering entity %llu:", entity.getId());
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
                "  Transform: pos=(%.1f, %.1f), rot=%.1f, scale=(%.1f, %.1f)",
                transform->getPosition().x, transform->getPosition().y,
                transform->getRotation(), transform->getScale().x,
                transform->getScale().y);
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
                "  Sprite: size=%.1fx%.1f, visible=%d, color=(%d,%d,%d,%d)",
                sprite->getWidth(), sprite->getHeight(), sprite->isVisible(),
                sprite->getColor().r, sprite->getColor().g,
                sprite->getColor().b, sprite->getColor().a);

    if (!sprite->isVisible()) {
      SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
                  "  Skipping entity %llu - not visible", entity.getId());
      continue;
    }

    // Get the position and size
    float x = transform->getPosition().x;
    float y = transform->getPosition().y;
    float width = sprite->getWidth() * transform->getScale().x;
    float height = sprite->getHeight() * transform->getScale().y;

    // Create rectangle for positioning
    SDL_FRect rect = {x, y, width, height};

    // If entity has images component, render the current image
    if (images) {
      const std::string &currentImageName = images->getCurrentImageName();
      SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
                  "  Images component found, current image: %s",
                  currentImageName.c_str());

      auto &resourceManager = resources::ResourceManager::getInstance();
      auto image = resourceManager.loadImage(currentImageName, renderer_);
      if (image) {
        // Draw the image centered in the rect
        image->render(renderer_, x, y, width, height, transform->getRotation());
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
                    "  Rendered image '%s' for entity %llu with rotation %.1f",
                    currentImageName.c_str(), entity.getId(),
                    transform->getRotation());
      } else {
        // Fall back to sprite if image loading failed
        drawSprite(sprite, rect, transform->getRotation());
      }
    } else {
      // Use sprite component if no images
      drawSprite(sprite, rect, transform->getRotation());
    }
  }
}

void RenderSystem::drawSprite(const components::Sprite *sprite,
                              const SDL_FRect &rect, float rotation) {
  // Set the color
  SDL_SetRenderDrawColor(renderer_, sprite->getColor().r, sprite->getColor().g,
                         sprite->getColor().b, sprite->getColor().a);

  // Draw the rectangle
  SDL_RenderFillRect(renderer_, &rect);

  SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
              "  Drew rectangle at (%.1f, %.1f) with size %.1fx%.1f", rect.x,
              rect.y, rect.w, rect.h);
}

std::string RenderSystem::toString() const {
  return "RenderSystem(entities=" + std::to_string(getEntities().size()) +
         ", renderer=" + (renderer_ ? "set" : "null") + ")";
}

} // namespace systems
} // namespace ecs
} // namespace game