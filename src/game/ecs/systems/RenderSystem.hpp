#pragma once

#include "../System.hpp"
#include "../Vector2.hpp"
#include <SDL3/SDL.h>
#include <memory>

namespace game {
namespace ecs {
class Entity;
namespace components {
class Sprite;
}
namespace systems {

/**
 * System that renders entities with sprite components.
 * Can optionally use Images component for image-based rendering.
 *
 * Based on: Lesson-40-WorldState/Python/src/game/ecs/systems/render_system.py
 *           Lesson-40-WorldState/Java/src/game/ecs/systems/RenderSystem.java
 */
class RenderSystem : public System {
public:
  /**
   * Creates a new RenderSystem.
   * @param renderer SDL renderer for drawing operations
   * @param backgroundColor Background color as SDL_Color (optional)
   */
  explicit RenderSystem(SDL_Renderer *renderer = nullptr,
                        const SDL_Color &backgroundColor = {0, 0, 0, 255});

  /**
   * Virtual destructor for proper cleanup
   */
  virtual ~RenderSystem() = default;

  /**
   * Set the graphics renderer for this render system.
   * @param renderer SDL renderer to use for drawing
   */
  void setRenderer(SDL_Renderer *renderer);

  /**
   * Update the render system.
   * Renders all entities with sprite components.
   * @param deltaTime Time elapsed since last update
   */
  void update(float deltaTime) override;

  /**
   * String representation for debugging
   * @return String describing the render system
   */
  std::string toString() const;

private:
  /**
   * Draw a sprite at the specified position and size.
   * @param sprite The sprite component to draw
   * @param rect The rectangle to draw in
   */
  void drawSprite(const components::Sprite *sprite, const SDL_FRect &rect,
                  float rotation = 0.0f);

  // SDL renderer for drawing operations
  SDL_Renderer *renderer_;

  // Background color for clearing the screen
  SDL_Color backgroundColor_;
};

} // namespace systems
} // namespace ecs
} // namespace game