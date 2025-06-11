#pragma once

#include "../Component.hpp"
#include "../Vector2.hpp"
#include <cmath>
#include <limits>

namespace game {
namespace ecs {
namespace components {

class Movement : public Component {
public:
    Movement(const Entity& entity,
             const Vector2& velocity = Vector2(),
             const Vector2& acceleration = Vector2())
        : Component(entity)
        , velocity_(velocity)
        , acceleration_(acceleration)
        , maxSpeed_(std::numeric_limits<float>::infinity())
        , enabled_(true) {}

    // Getters
    const Vector2& getVelocity() const { return velocity_; }
    const Vector2& getAcceleration() const { return acceleration_; }
    float getMaxSpeed() const { return maxSpeed_; }
    bool isEnabled() const { return enabled_; }

    // Setters
    void setVelocity(const Vector2& velocity) { 
        velocity_ = velocity;
        clampVelocity();
    }
    void setVelocity(float x, float y) { 
        velocity_ = Vector2(x, y);
        clampVelocity();
    }
    void setAcceleration(const Vector2& acceleration) { acceleration_ = acceleration; }
    void setAcceleration(float x, float y) { acceleration_ = Vector2(x, y); }
    void setMaxSpeed(float maxSpeed) { 
        maxSpeed_ = maxSpeed;
        clampVelocity();
    }
    void setEnabled(bool enabled) { enabled_ = enabled; }

    void enable() {
        enabled_ = true;
    }

    void disable() {
        enabled_ = false;
        velocity_ = Vector2();
        acceleration_ = Vector2();
    }

    void applyAcceleration(float deltaTime) {
        velocity_ += acceleration_ * deltaTime;
        clampVelocity();
    }

private:
    void clampVelocity() {
        if (maxSpeed_ == std::numeric_limits<float>::infinity()) {
            return;
        }

        float speedSquared = velocity_.x * velocity_.x + velocity_.y * velocity_.y;
        if (speedSquared > maxSpeed_ * maxSpeed_) {
            float scale = maxSpeed_ / std::sqrt(speedSquared);
            velocity_ = Vector2(velocity_.x * scale, velocity_.y * scale);
        }
    }

    Vector2 velocity_;
    Vector2 acceleration_;
    float maxSpeed_;
    bool enabled_;
};

} // namespace components
} // namespace ecs
} // namespace game 