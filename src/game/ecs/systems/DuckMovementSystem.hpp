#pragma once

#include "../System.hpp"
#include "../Vector2.hpp"
#include <SDL3/SDL.h>

namespace game {
namespace ecs {
namespace systems {

/**
 * System that handles duck movement across the screen.
 * Ducks fly horizontally and are marked expired when they reach the opposite edge.
 * 
 * Based on: Lesson-40-WorldState/Python/src/game/ecs/systems/duck_movement_system.py
 *           Lesson-40-WorldState/Java/src/game/ecs/systems/DuckMovementSystem.java
 */
class DuckMovementSystem : public System {
public:
    /**
     * Creates a new DuckMovementSystem.
     * @param worldWidth Width of the game world
     * @param worldHeight Height of the game world
     */
    DuckMovementSystem(float worldWidth, float worldHeight);

    /**
     * Virtual destructor for proper cleanup
     */
    virtual ~DuckMovementSystem() = default;

    /**
     * Update duck movement for all duck entities.
     * @param deltaTime Time elapsed since last update
     */
    void update(float deltaTime) override;

    /**
     * String representation for debugging
     * @return String describing the movement system
     */
    std::string toString() const;

private:
    /**
     * Update sprite direction based on velocity if Images component is present.
     * @param entity The entity to update
     * @param velocityX The X velocity to determine direction
     */
    void updateSpriteDirection(const Entity& entity, float velocityX);

    // World dimensions
    float worldWidth_;
    float worldHeight_;

    // Duck constants
    static constexpr float DUCK_WIDTH = 40.0f;     // Duck sprite width
    static constexpr float EDGE_MARGIN = 25.0f;    // Margin beyond screen edge
};

} // namespace systems
} // namespace ecs
} // namespace game 