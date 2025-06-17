#pragma once

#include <SDL3/SDL.h>
#include <vector>
#include <memory>
#include <unordered_map>
#include "TextRenderer.hpp"
#include "../events/EventListener.hpp"
#include "../GameColor.hpp"
#include "../Timer.hpp"

namespace game {

// Forward declarations
namespace ecs {
namespace components {
    class Collision;
}
}

namespace ui {

/**
 * DebugOverlay - Development and debugging tools component.
 * 
 * Provides debugging information and tools for development:
 * - F1: Toggle debug overlay visibility
 * - F2: Toggle collision information
 * - F3: Toggle performance information
 * - F4: Toggle entity information
 * 
 * Based on: Lesson-40-WorldState/Documentation/NewHudDesign.md
 * Follows: Professional 4-component HUD architecture
 */
class DebugOverlay : public events::EventListener {
public:
    /**
     * Constructor
     * @param screenWidth Width of the screen
     * @param screenHeight Height of the screen
     * @param timer Timer instance for FPS calculation
     */
    DebugOverlay(int screenWidth, int screenHeight, Timer* timer);

    /**
     * Destructor
     */
    ~DebugOverlay();

    /**
     * Update debug information
     * @param deltaTime Time elapsed since last frame
     */
    void update(float deltaTime);

    /**
     * Render debug information to the screen
     * @param renderer SDL renderer
     */
    void render(SDL_Renderer* renderer);

    /**
     * Render collision debug information
     * @param renderer SDL renderer
     * @param collisionComponents Map of collision components
     */
    void renderCollisionInfo(SDL_Renderer* renderer,
                           const std::unordered_map<std::string, ecs::components::Collision*>& collisionComponents);

    /**
     * Toggle debug overlay visibility
     */
    void toggleVisibility();

    /**
     * Set debug overlay visibility
     * @param visible Whether overlay should be visible
     */
    void setVisible(bool visible);

    /**
     * Check if debug overlay is visible
     * @return true if visible
     */
    bool isVisible() const;

    /**
     * Toggle collision information display
     */
    void toggleCollisionInfo();

    /**
     * Toggle performance information display
     */
    void togglePerformanceInfo();

    /**
     * Toggle entity information display
     */
    void toggleEntityInfo();

    /**
     * Handle events (EventListener interface)
     * @param event The event to handle
     */
    void onEvent(const events::Event& event) override;

    /**
     * Get the TextRenderer for shared use
     * @return Reference to TextRenderer
     */
    TextRenderer& getTextRenderer();

private:
    int screenWidth;
    int screenHeight;
    Timer* timer;
    
    // Visibility flags
    bool visible;
    bool collisionInfoVisible;
    bool performanceVisible;
    bool entityInfoVisible;
    
    // Components
    std::unique_ptr<TextRenderer> textRenderer;
    
    // No local performance tracking needed - Timer provides FPS
    
    // Colors (based on design specification)
    GameColor primaryTextColor;      // Black (0, 0, 0)
    GameColor secondaryTextColor;    // Black (0, 0, 0)
    GameColor warningColor;          // Orange (255, 165, 0)
    GameColor errorColor;            // Red-orange (255, 69, 0)
    GameColor successColor;          // Black (0, 0, 0)
    GameColor backgroundColor;       // Light blue with alpha (173, 216, 230, 190)
    
    /**
     * Draw semi-transparent background
     * @param renderer SDL renderer
     */
    void drawBackground(SDL_Renderer* renderer);
    
    /**
     * Render debug help/controls
     * @param renderer SDL renderer
     */
    void renderDebugHelp(SDL_Renderer* renderer);
    
    /**
     * Render performance monitoring information
     * @param renderer SDL renderer
     */
    void renderPerformanceInfo(SDL_Renderer* renderer);
    
    /**
     * Render entity count and system information
     * @param renderer SDL renderer
     */
    void renderEntityInfo(SDL_Renderer* renderer);
};

} // namespace ui
} // namespace game 