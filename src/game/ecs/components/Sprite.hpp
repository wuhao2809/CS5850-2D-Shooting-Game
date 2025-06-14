#pragma once

#include "../Component.hpp"
#include <SDL3/SDL.h>

namespace game {
namespace ecs {
namespace components {

class Sprite : public Component {
public:
    Sprite(const Entity& entity,
           float width,
           float height,
           const SDL_Color& color = {255, 255, 255, 255})
        : Component(entity)
        , width_(width)
        , height_(height)
        , color_(color)
        , visible_(true) {}

    // Getters
    float getWidth() const { return width_; }
    float getHeight() const { return height_; }
    const SDL_Color& getColor() const { return color_; }
    bool isVisible() const { return visible_; }

    // Setters
    void setWidth(float width) { width_ = width; }
    void setHeight(float height) { height_ = height; }
    void setColor(const SDL_Color& color) { color_ = color; }
    void setColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) {
        color_ = {r, g, b, a};
    }
    void setVisible(bool visible) { visible_ = visible; }

private:
    float width_;
    float height_;
    SDL_Color color_;
    bool visible_;
};

} // namespace components
} // namespace ecs
} // namespace game 