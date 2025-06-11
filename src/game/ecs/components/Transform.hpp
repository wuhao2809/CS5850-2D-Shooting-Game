#pragma once

#include "../Component.hpp"
#include "../Vector2.hpp"

namespace game {
namespace ecs {
namespace components {

class Transform : public Component {
public:
    Transform(const Entity& entity, 
             const Vector2& position = Vector2(),
             float rotation = 0.0f,
             const Vector2& scale = Vector2(1.0f, 1.0f))
        : Component(entity)
        , position_(position)
        , rotation_(rotation)
        , scale_(scale) {}

    // Getters
    const Vector2& getPosition() const { return position_; }
    float getRotation() const { return rotation_; }
    const Vector2& getScale() const { return scale_; }

    // Setters
    void setPosition(const Vector2& position) { position_ = position; }
    void setPosition(float x, float y) { position_ = Vector2(x, y); }
    void setRotation(float rotation) { rotation_ = rotation; }
    void setScale(const Vector2& scale) { scale_ = scale; }
    void setScale(float x, float y) { scale_ = Vector2(x, y); }

private:
    Vector2 position_;
    float rotation_;
    Vector2 scale_;
};

} // namespace components
} // namespace ecs
} // namespace game 