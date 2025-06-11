#include "GameEngine.hpp"
#include <SDL3/SDL.h>
#include <iostream>
#include <stdexcept>

namespace game {

/**
 * @brief Constructs a GameEngine with specified window parameters
 * Initializes member variables but does not create SDL resources
 */
GameEngine::GameEngine(int width, int height, const std::string &title)
    : window(nullptr), renderer(nullptr), width(width), height(height),
      title(title), running(false), backgroundColor(255, 255, 255) {}

/**
 * @brief Destructor that ensures proper cleanup of SDL resources
 * Calls destroy() to clean up SDL resources
 */
GameEngine::~GameEngine() { destroy(); }

/**
 * @brief Initializes SDL and creates window and renderer
 * Sets up all necessary SDL components for the game engine
 *
 * @return true if initialization was successful, false otherwise
 */
bool GameEngine::init() {
  std::cout << "Initializing SDL..." << std::endl;
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
    return false;
  }

  std::cout << "Creating window..." << std::endl;
  window = SDL_CreateWindow(title.c_str(), width, height, SDL_WINDOW_RESIZABLE);
  if (!window) {
    std::cerr << "Window Creation Error: " << SDL_GetError() << std::endl;
    SDL_Quit();
    return false;
  }

  std::cout << "Creating renderer..." << std::endl;
  renderer = SDL_CreateRenderer(window, nullptr);
  if (!renderer) {
    std::cerr << "Renderer Creation Error: " << SDL_GetError() << std::endl;
    SDL_DestroyWindow(window);
    SDL_Quit();
    return false;
  }

  running = true;
  std::cout << "Initialization complete!" << std::endl;
  return true;
}

/**
 * @brief Main game loop
 * Executes the three core methods in sequence:
 * 1. Process input (handleEvents)
 * 2. Update game state (update)
 * 3. Display/rendering (display)
 *
 * The loop continues until running is set to false
 */
void GameEngine::run() {
  while (running) {
    handleEvents();
    update();
    display();
  }
}

/**
 * @brief Process input events from SDL
 * Handles window events and keyboard input
 * Currently only handles the quit event
 */
void GameEngine::handleEvents() {
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    if (event.type == SDL_EVENT_QUIT) {
      std::cout << "Window close event received" << std::endl;
      running = false;
    } else if (event.type == SDL_EVENT_KEY_DOWN) {
      std::cout << "Key pressed: " << SDL_GetKeyName(event.key.key)
                << " (scancode: " << SDL_GetScancodeName(event.key.scancode)
                << ")" << std::endl;
      if (event.key.key == SDLK_Q) {
        std::cout << "Quit key (Q) pressed" << std::endl;
        running = false;
      }
    }
  }
}

/**
 * @brief Update game state
 * Currently empty as we only have a static background
 * This is where game logic like physics, AI, and state changes would go
 */
void GameEngine::update() {
  // Game logic updates go here
}

/**
 * @brief Display/rendering phase of the game loop
 * Clears the screen with the background color and presents it
 */
void GameEngine::display() {
  clear(backgroundColor);
  present();
}

/**
 * @brief Stops the game loop
 * Sets running to false, which will exit the game loop
 */
void GameEngine::quit() { running = false; }

/**
 * @brief Clear the screen with specified color
 * @param color The color to clear the screen with
 */
void GameEngine::clear(const GameColor &color) {
  SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
  SDL_RenderClear(renderer);
}

/**
 * @brief Present the rendered frame to the screen
 * Updates the window with the current renderer state
 */
void GameEngine::present() { SDL_RenderPresent(renderer); }

/**
 * @brief Clean up SDL resources
 * Destroys the renderer, window, and quits SDL
 * Called by the destructor and on initialization failure
 */
void GameEngine::destroy() {
  if (renderer) {
    SDL_DestroyRenderer(renderer);
    renderer = nullptr;
  }
  if (window) {
    SDL_DestroyWindow(window);
    window = nullptr;
  }
  SDL_Quit();
}
} // namespace game