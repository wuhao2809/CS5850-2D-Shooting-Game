#pragma once

#include <SDL3/SDL.h>
#ifdef USE_SDL3_TTF
#include <SDL3_ttf/SDL_ttf.h>
#endif
#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <unordered_map>
#include "GameColor.hpp"
#include "Timer.hpp"
#include "ui/HUD.hpp"
#include "GameWorld.hpp"
#include "events/Event.hpp"
#include "events/KeyboardEvent.hpp"
#include "events/EventManager.hpp"

namespace game {
    /**
     * GameEngine class that manages the game loop and rendering.
     * 
     * Key Components:
     * 1. SDL Window and Renderer: Handles the game window and rendering
     * 2. Timer: Controls frame rate and timing
     * 3. HUD: Displays game information
     * 4. GameWorld: Manages game objects and world state
     * 5. EventManager: Handles event distribution and processing
     * 
     * Game Loop:
     * 1. Process input (handleEvents)
     * 2. Update game state (update)
     * 3. Render frame (display)
     * 
     * Rendering Process:
     * 1. Clear screen with background color
     * 2. Render all game objects
     * 3. Render HUD
     * 4. Present frame to screen
     */
    class GameEngine {
    public:
        /**
         * Constructor for GameEngine
         * 
         * @param width Window width in pixels
         * @param height Window height in pixels
         * @param title Window title
         * @param assetsDir Path to the assets directory
         */
        GameEngine( 
                  const std::string& title = "Game Engine",
                  const std::string& assetsDir = "GameAssets");
        
        /**
         * Destructor that ensures proper cleanup of SDL resources
         */
        ~GameEngine();

        /**
         * Initialize SDL and create window and renderer
         * 
         * @return true if initialization was successful
         */
        bool init();
        
        /**
         * Start the game loop
         */
        void run();
        
        /**
         * Stop the game loop
         */
        void quit();

    private:
        SDL_Window* window;      // SDL window
        SDL_Renderer* renderer;  // SDL renderer
#ifdef USE_SDL3_TTF
        TTF_Font* font;          // Font for text rendering
        SDL_Surface* textSurface;  // Surface for text
        SDL_Texture* textTexture;  // Texture for text
#endif
        int width;               // Window width
        int height;              // Window height
        std::string title;       // Window title
        bool running;            // Game loop running flag
        Timer timer;             // Frame rate control
        std::unique_ptr<HUD> hud;  // Heads-up display
        GameWorld* gameWorld;    // Game world instance
        std::string assetsDirectory;   // Path to assets directory

        /**
         * Process input events from SDL
         */
        void handleEvents();
        
        /**
         * Update game state
         */
        void update();
        
        /**
         * Render the current frame
         */
        void display();
        
        /**
         * Clear the screen with specified color
         * 
         * @param color Color to clear with
         */
        void clear(const GameColor& color);
        
        /**
         * Present the rendered frame to the screen
         */
        void present();
        
        /**
         * Clean up SDL resources
         */
        void destroy();
        

#ifdef USE_SDL3_TTF
        /**
         * Render text at specified position
         * 
         * @param text Text to render
         * @param x X coordinate
         * @param y Y coordinate
         */
        void renderText(const std::string& text, int x, int y);
#endif
    };
} 