#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include "../GameColor.hpp"
#include "../events/EventListener.hpp"
#include "GameHUD.hpp"
#include "DebugOverlay.hpp"
#include "../Timer.hpp"

namespace game {

// Forward declarations
namespace ecs {
namespace components {
    class Collision;
}
}

/**
 * HUD Master Coordinator - Professional 4-component HUD architecture.
 * 
 * Coordinates all HUD components and provides a unified interface:
 * - GameHUD for professional game interface (score, time, etc.)
 * - DebugOverlay for development information (F1-F4 toggles)
 * - TextRenderer for efficient text rendering (shared between components)
 * - HUD Master Class for coordination and backward compatibility
 * 
 * Based on: Lesson-40-WorldState/Documentation/NewHudDesign.md
 * Follows: Python/Java reference implementations
 */
class HUD : public events::EventListener {
public:
    /**
     * Constructor
     * @param screenWidth Width of the screen
     * @param screenHeight Height of the screen
     * @param timer Timer instance for FPS calculation
     * @param enableDebug Whether to enable debug overlay (set to false for release)
     */
    HUD(int screenWidth, int screenHeight, Timer* timer, bool enableDebug = true);

    /**
     * Destructor
     */
    ~HUD();

    /**
     * Update all HUD components (called each frame)
     * @param deltaTime Time since last frame
     */
    void update(float deltaTime);

    /**
     * Render all HUD components
     * @param renderer SDL renderer
     */
    void render(SDL_Renderer* renderer);

    /**
     * Render collision debug information
     * @param renderer SDL renderer
     * @param collisionComponents Map of collision components
     */
    void renderCollisionDebug(SDL_Renderer* renderer, 
                              const std::unordered_map<std::string, ecs::components::Collision*>& collisionComponents);

    /**
     * Toggle HUD visibility
     */
    void toggleVisibility();

    /**
     * Set HUD visibility
     * @param visible Whether HUD should be visible
     */
    void setVisible(bool visible);

    /**
     * Check if HUD is visible
     * @return true if visible
     */
    bool isVisible() const;

    /**
     * Get access to the professional GameHUD
     * @return Reference to GameHUD
     */
    ui::GameHUD& getGameHUD();

    /**
     * Get access to the DebugOverlay (may be null if debug disabled)
     * @return Pointer to DebugOverlay
     */
    ui::DebugOverlay* getDebugOverlay();

    /**
     * Check if debug features are enabled
     * @return true if debug is enabled
     */
    bool isDebugEnabled() const;

    // Convenience methods for controlling HUD components

    /**
     * Show the game HUD
     */
    void showGameHUD();

    /**
     * Hide the game HUD
     */
    void hideGameHUD();

    /**
     * Toggle game HUD visibility
     */
    void toggleGameHUD();

    /**
     * Show debug overlay (if enabled)
     */
    void showDebug();

    /**
     * Hide debug overlay (if enabled)
     */
    void hideDebug();

    /**
     * Toggle debug overlay visibility (if enabled)
     */
    void toggleDebug();

    /**
     * Handle events (EventListener interface)
     * @param event The event to handle
     */
    void onEvent(const events::Event& event) override;

private:
    int screenWidth;
    int screenHeight;
    bool enableDebug;
    bool visible;

    // HUD Components (4-component professional architecture)
    std::unique_ptr<ui::GameHUD> gameHUD;
    std::unique_ptr<ui::DebugOverlay> debugOverlay;

    // Debug rendering
    std::unordered_map<std::string, ecs::components::Collision*> currentCollisions;
};

} // namespace game 