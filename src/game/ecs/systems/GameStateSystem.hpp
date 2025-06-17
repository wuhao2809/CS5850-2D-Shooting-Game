#pragma once

#include "../System.hpp"
#include "../components/ShootingGalleryState.hpp"
#include "../components/CollisionResult.hpp"
#include <SDL3/SDL.h>

namespace game {
namespace ecs {
namespace systems {

/**
 * Enum representing the possible states of the game.
 */
enum class GameState {
    RUNNING,
    GAME_OVER
};

/**
 * System that manages the game state and handles state transitions.
 * 
 * Uses CollisionResult components to detect player collisions for game over state.
 * Also manages the game timer for 60-second rounds.
 * 
 * C++ Implementation References:
 * - Python: /Lesson-41-CleanEvents/Python/src/game/ecs/systems/game_state_system.py
 * - Java: /Lesson-41-CleanEvents/Java/src/game/ecs/systems/GameStateSystem.java
 */
class GameStateSystem : public System {
public:
    /**
     * Constructor for GameStateSystem.
     */
    explicit GameStateSystem();

    /**
     * Virtual destructor for proper cleanup
     */
    virtual ~GameStateSystem() = default;

    /**
     * Update the game state system.
     * @param deltaTime Time elapsed since last update
     */
    void update(float deltaTime) override;

    /**
     * Check if the game is in game over state.
     * @return true if the game is over, false otherwise
     */
    bool isGameOver() const;

    /**
     * Reset the game state to running.
     */
    void reset();

    /**
     * Get the current game state.
     * @return The current game state
     */
    GameState getState() const;

private:
    /**
     * Process collision results to detect player collisions that should end the game.
     * 
     * Only handles player-related collisions that should end the game.
     * Projectile-target collisions are handled by ProjectileSystem for scoring.
     */
    void processCollisionResults();

    GameState state_;
};

} // namespace systems
} // namespace ecs
} // namespace game 