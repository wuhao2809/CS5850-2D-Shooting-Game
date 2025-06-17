#pragma once

#include "../System.hpp"
#include "../components/ShootingGalleryState.hpp"
#include "../Vector2.hpp"
#include <random>
#include <unordered_map>
#include <string>
#include <SDL3/SDL.h>
#include <nlohmann/json.hpp>

namespace game {
namespace ecs {
namespace systems {

/**
 * System that spawns targets at regular intervals with different types and behaviors.
 * Manages target variety and positioning in the shooting gallery.
 * 
 * Based on: Lesson-40-WorldState/Python/src/game/ecs/systems/target_spawn_system.py
 *           Lesson-40-WorldState/Java/src/game/ecs/systems/TargetSpawnSystem.java
 */
class TargetSpawnSystem : public System {
public:
    /**
     * Creates a new TargetSpawnSystem.
     * @param worldWidth Width of the game world for positioning
     * @param worldHeight Height of the game world for positioning
     */
    TargetSpawnSystem(float worldWidth, float worldHeight);

    /**
     * Virtual destructor for proper cleanup
     */
    virtual ~TargetSpawnSystem() = default;

    /**
     * Set the duck templates loaded from JSON.
     * @param templates Map of template name to JSON template data
     */
    void setTemplates(const std::unordered_map<std::string, nlohmann::json>& templates);

    /**
     * Update target spawning based on game state.
     * @param deltaTime Time elapsed since last update
     */
    void update(float deltaTime) override;

    /**
     * String representation for debugging
     * @return String describing the spawn system
     */
    std::string toString() const;

private:
    /**
     * Count the number of active ducks currently on screen.
     * @return Number of active (non-expired) duck entities
     */
    int countActiveDucks() const;

    /**
     * Spawn a new duck at the left or right edge that will fly horizontally across the screen.
     */
    void spawnTarget();

    /**
     * Choose a target type based on weighted probabilities.
     * @return The chosen target type ('regular' or 'boss')
     */
    std::string chooseTargetType();

    /**
     * Get the flight level (y position) from the template.
     * @param templateName Name of the duck template
     * @return The y position for this duck type
     */
    float getFlightLevelFromTemplate(const std::string& templateName) const;

    /**
     * Get the speed from the template's speed component.
     * @param templateName Name of the duck template
     * @return The duck speed
     */
    float getSpeedFromTemplate(const std::string& templateName) const;

    /**
     * Create a duck entity using a JSON template.
     * @param entity The entity to configure
     * @param x X position
     * @param y Y position
     * @param direction Direction of movement (1 for right, -1 for left)
     * @param templateName Name of the template to use
     */
    void createDuckFromTemplate(const Entity& entity, float x, float y, float direction, 
                               const std::string& templateName);

    // World dimensions
    float worldWidth_;
    float worldHeight_;

    // Spawn configuration
    static constexpr float SPAWN_AREA_TOP = 50.0f;       // Top of spawn area
    static constexpr float SPAWN_MARGIN = 50.0f;         // Margin from edges
    float spawnAreaBottom_;                               // Bottom of spawn area (60% from top)

    // Random number generation
    mutable std::random_device randomDevice_;
    mutable std::mt19937 randomEngine_;
    mutable std::uniform_real_distribution<float> distribution_;

    // Target type probabilities (should sum to 1.0)
    std::unordered_map<std::string, float> targetWeights_;

    // Template storage - loaded from JSON
    std::unordered_map<std::string, nlohmann::json> templates_;
};

} // namespace systems
} // namespace ecs
} // namespace game 