#pragma once

#include <memory>
#include <SDL3/SDL.h>
#include "TextRenderer.hpp"
#include "../GameColor.hpp"

// Forward declaration
namespace game {
namespace ecs {
namespace components {
    class ShootingGalleryState;
}
}
}

namespace game {
namespace ui {

/**
 * Game HUD - Displays game status information like score, time, health, etc.
 * This is the actual game interface that players see during gameplay.
 * 
 * Based on: Lesson-40-WorldState/Python/src/game/ui/game_hud.py
 *           Lesson-40-WorldState/Java/src/game/ui/GameHUD.java
 */
class GameHUD {
public:
    /**
     * Initialize the Game HUD.
     * 
     * @param screenWidth Width of the game window
     * @param screenHeight Height of the game window
     */
    GameHUD(int screenWidth, int screenHeight);
    
    /**
     * Destructor
     */
    ~GameHUD();
    
    /**
     * Update the HUD state.
     * 
     * @param deltaTime Time elapsed since last update
     */
    void update(float deltaTime);
    
    /**
     * Render the game HUD to the screen.
     * 
     * @param renderer SDL renderer to render to
     */
    void render(SDL_Renderer* renderer);
    
    /**
     * Toggle HUD visibility
     */
    void toggleVisibility();
    
    /**
     * Set HUD visibility
     * 
     * @param visible Whether the HUD should be visible
     */
    void show();
    
    /**
     * Hide the HUD
     */
    void hide();
    
    /**
     * Check if the HUD is visible
     * 
     * @return Whether the HUD is visible
     */
    bool isVisible() const;
    
    /**
     * Get the TextRenderer instance
     * 
     * @return Reference to the text renderer
     */
    TextRenderer& getTextRenderer();
    
    /**
     * Cleanup resources
     */
    void cleanup();

private:
    int screenWidth;
    int screenHeight;
    std::unique_ptr<TextRenderer> textRenderer;
    
    // Game state reference
    ecs::components::ShootingGalleryState* gameState;
    
    // HUD visibility
    bool visible;
    
    // Colors
    GameColor normalColor;   // White
    GameColor warningColor;  // Yellow
    GameColor criticalColor; // Red
    GameColor goodColor;     // Green
    
    /**
     * Render HUD elements during gameplay.
     * 
     * @param renderer SDL renderer to render to
     */
    void renderGameplayHUD(SDL_Renderer* renderer);
    
    /**
     * Render HUD elements during game over screen.
     * 
     * @param renderer SDL renderer to render to
     */
    void renderGameOverHUD(SDL_Renderer* renderer);
    
    /**
     * Render HUD elements during menu screen.
     * 
     * @param renderer SDL renderer to render to
     */
    void renderMenuHUD(SDL_Renderer* renderer);
    
    /**
     * Get color for time display based on remaining time.
     * 
     * @param timeRemaining Time remaining in seconds
     * @return Color for time display
     */
    GameColor getTimeColor(float timeRemaining);
    
    /**
     * Get color for accuracy display based on accuracy percentage.
     * 
     * @param accuracy Accuracy as percentage (0-100)
     * @return Color for accuracy display
     */
    GameColor getAccuracyColor(float accuracy);
};

} // namespace ui
} // namespace game 