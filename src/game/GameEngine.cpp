#include "GameEngine.hpp"
#include <SDL3/SDL.h>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <stdexcept>

namespace game {
using namespace events; // Add this to simplify event-related code

/**
 * @brief Constructs a GameEngine with specified window parameters
 * Initializes member variables but does not create SDL resources
 */
GameEngine::GameEngine(const std::string &title, const std::string &assetsDir)
#ifdef USE_SDL3_TTF
    : window(nullptr), renderer(nullptr), font(nullptr), textSurface(nullptr),
      textTexture(nullptr), width(800), height(600), title(title),
      assetsDirectory(assetsDir), running(false), timer(60),
      hud(std::make_unique<HUD>(width, height)), gameWorld(nullptr){}
#else
    : window(nullptr), renderer(nullptr), width(800), height(600), title(title),
      assetsDirectory(assetsDir), running(false), timer(60),
      hud(std::make_unique<HUD>(width, height)), gameWorld(nullptr) {
}
#endif

      /**
       * @brief Destructor that ensures proper cleanup of SDL resources
       * Calls destroy() to clean up SDL resources
       */
      GameEngine::~GameEngine() {
  destroy();
}

/**
 * @brief Initializes SDL and creates window and renderer
 * Sets up all necessary SDL components for the game engine
 *
 * @return true if initialization was successful, false otherwise
 */
bool GameEngine::init() {
  SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Initializing SDL...");
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL_Init Error: %s", SDL_GetError());
    return false;
  }

#ifdef USE_SDL3_TTF
  SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Initializing TTF...");
  if (!TTF_Init()) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "TTF_Init Error: %s", SDL_GetError());
    SDL_Quit();
    return false;
  }

  SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Loading font...");
#ifdef __APPLE__
  font = TTF_OpenFont("/System/Library/Fonts/Helvetica.ttc",
                      24); // Use Helvetica on macOS
#else
  font =
      TTF_OpenFont("C:\\Windows\\Fonts\\arial.ttf", 24); // Use Arial on Windows
#endif
  if (!font) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Font Loading Error: %s",
                 SDL_GetError());
    TTF_Quit();
    SDL_Quit();
    return false;
  }
#endif
  // Initialize GameWorld
  SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Initializing GameWorld...");
  gameWorld = &GameWorld::getInstance();
  SDL_Log("[GameEngine] Setting renderer in GameWorld: %p", renderer);

  gameWorld->setAssetsDirectory(
      assetsDirectory);    // Set assets directory before initialization
  gameWorld->initialize(); // Initialize first

  // Get world dimensions from GameWorld after initialization
  width = gameWorld->getWorldWidth();
  height = gameWorld->getWorldHeight();
  SDL_Log("[GameEngine] World dimensions from GameWorld: %dx%d", width, height);

  SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Creating window...");
  window = SDL_CreateWindow(title.c_str(), width, height, SDL_WINDOW_RESIZABLE);
  if (!window) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Window Creation Error: %s",
                 SDL_GetError());
    SDL_Quit();
    return false;
  }

  SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Creating renderer...");
  renderer = SDL_CreateRenderer(window, nullptr);
  if (!renderer) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Renderer Creation Error: %s",
                 SDL_GetError());
    SDL_DestroyWindow(window);
    SDL_Quit();
    return false;
  }
  gameWorld->setRenderer(renderer); // Set renderer after creation

  // Set up HUD elements
  if (hud) {
    // Add FPS counter with black color
    hud->addText("fps", "FPS: 0", 10, 10, 16);
    hud->setColor("fps", GameColor(0, 0, 0, 255));

    // Add log level display with black color
    hud->addText("logLevel", "Log Level: INFO", 10, height - 30, 24);
    hud->setColor("logLevel", GameColor(0, 0, 0, 255));
  }

  running = true;
  SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Initialization complete!");

  return true;
}

/**
 * @brief Main game loop
 * Executes the three core methods in sequence:
 * 1. Process input (handleEvents)
 * 2. Update game state (update)
 * 3. Display/rendering (render)
 *
 * Uses Timer for frame rate control
 */
void GameEngine::run() {
  while (running) {
    timer.startFrame();
    clear(GameColor::BACKGROUND);
    handleEvents();
    update();
    display();

    timer.waitForFrameEnd();
  }
}

/**
 * @brief Process input events from SDL
 * Handles window events and keyboard input
 * Currently handles quit event and 'Q' key for exit
 */
void GameEngine::handleEvents() {
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    if (event.type == SDL_EVENT_QUIT) {
      running = false;
    } else if (event.type == SDL_EVENT_KEY_DOWN) {
      // Create and publish keyboard event for key press
      const char *keyName = SDL_GetKeyName(event.key.key);
      std::string keyNameLower = std::string(keyName);
      std::transform(keyNameLower.begin(), keyNameLower.end(),
                     keyNameLower.begin(), ::tolower);
      SDL_LogVerbose(SDL_LOG_CATEGORY_INPUT,
                     "[GameEngine] Key pressed: %s (SDL key code: %d)",
                     keyNameLower.c_str(), event.key.key);

      auto keyboardEvent = std::make_shared<events::KeyboardEvent>(
          keyNameLower, keyNameLower, true);
      SDL_LogWarn(
          SDL_LOG_CATEGORY_INPUT,
          "[GameEngine] Publishing keyboard event - Key: %s, Pressed: true",
          keyNameLower.c_str());
      events::EventManager::getInstance().publish(keyboardEvent);

      if (event.key.key == SDLK_Q) {
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
                    "[GameEngine] Quit key (Q) pressed, stopping game loop");
        running = false;
      } else if (event.key.key == SDLK_H) {
        if (hud) {
          hud->toggleVisibility();
          SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
                      "[GameEngine] HUD visibility toggled");
        }
      } else if (event.key.key == SDLK_ESCAPE) {
        // Cycle through log levels
        static SDL_LogPriority logLevels[] = {
            SDL_LOG_PRIORITY_CRITICAL, // Most severe
            SDL_LOG_PRIORITY_ERROR,    SDL_LOG_PRIORITY_WARN,
            SDL_LOG_PRIORITY_INFO,     SDL_LOG_PRIORITY_DEBUG,
            SDL_LOG_PRIORITY_VERBOSE // Least severe
        };
        static int currentLogLevelIndex = 0;

        // Update all categories to the new level
        SDL_LogPriority newLevel = logLevels[currentLogLevelIndex];
        SDL_SetLogPriorities(newLevel);

        // Log the change
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
                    "[GameEngine] Log level changed to: %s",
                    newLevel == SDL_LOG_PRIORITY_CRITICAL  ? "CRITICAL"
                    : newLevel == SDL_LOG_PRIORITY_ERROR   ? "ERROR"
                    : newLevel == SDL_LOG_PRIORITY_WARN    ? "WARN"
                    : newLevel == SDL_LOG_PRIORITY_INFO    ? "INFO"
                    : newLevel == SDL_LOG_PRIORITY_DEBUG   ? "DEBUG"
                    : newLevel == SDL_LOG_PRIORITY_VERBOSE ? "VERBOSE"
                                                           : "UNKNOWN");

        // Update HUD if it exists
        if (hud) {
          std::stringstream ss;
          ss << "Log Level: "
             << (newLevel == SDL_LOG_PRIORITY_CRITICAL  ? "CRITICAL"
                 : newLevel == SDL_LOG_PRIORITY_ERROR   ? "ERROR"
                 : newLevel == SDL_LOG_PRIORITY_WARN    ? "WARN"
                 : newLevel == SDL_LOG_PRIORITY_INFO    ? "INFO"
                 : newLevel == SDL_LOG_PRIORITY_DEBUG   ? "DEBUG"
                 : newLevel == SDL_LOG_PRIORITY_VERBOSE ? "VERBOSE"
                                                        : "UNKNOWN");
          hud->updateText("logLevel", ss.str());
        }

        // Move to next level
        currentLogLevelIndex = (currentLogLevelIndex + 1) %
                               (sizeof(logLevels) / sizeof(logLevels[0]));
      }
    } else if (event.type == SDL_EVENT_KEY_UP) {
      // Create and publish keyboard event for key release
      const char *keyName = SDL_GetKeyName(event.key.key);
      std::string keyNameLower = std::string(keyName);
      std::transform(keyNameLower.begin(), keyNameLower.end(),
                     keyNameLower.begin(), ::tolower);
      SDL_LogVerbose(SDL_LOG_CATEGORY_INPUT,
                     "[GameEngine] Key released: %s (SDL key code: %d)",
                     keyNameLower.c_str(), event.key.key);

      auto keyboardEvent = std::make_shared<events::KeyboardEvent>(
          keyNameLower, keyNameLower, false);
      SDL_LogWarn(
          SDL_LOG_CATEGORY_INPUT,
          "[GameEngine] Publishing keyboard event - Key: %s, Pressed: false",
          keyNameLower.c_str());
      events::EventManager::getInstance().publish(keyboardEvent);
    }
  }

  // Process any queued events
  events::EventManager::getInstance().update();
}

/**
 * @brief Update game state
 * Updates all game objects and HUD elements
 */
void GameEngine::update() {
  // Update game objects through GameWorld
  gameWorld->update(timer.getAverageFrameTime());

  // Update FPS display
  if (hud) {
    std::stringstream ss;
    ss << "FPS: " << static_cast<int>(timer.getFps());
    hud->updateText("fps", ss.str());
  }
}

/**
 * @brief Display/rendering phase of the game loop
 *
 * This method handles the rendering sequence:
 * 1. Clear the screen with the background color
 * 2. Render all game objects from GameWorld
 * 3. Render the HUD if it exists
 * 4. Present the frame to the screen
 *
 * The rendering order is important:
 * - Background is drawn first
 * - Game objects are drawn on top of the background
 * - HUD is drawn last to ensure it's always visible
 */
void GameEngine::display() {
  SDL_LogDebug(SDL_LOG_CATEGORY_RENDER, "Starting display phase...");

  // Render HUD if it exists
  if (hud) {
    SDL_LogDebug(SDL_LOG_CATEGORY_RENDER, "Rendering HUD...");
    hud->render(renderer);
  }

  // Present the frame to the screen
  SDL_LogDebug(SDL_LOG_CATEGORY_RENDER, "Presenting frame...");
  present();

  // Check for SDL errors
  const char *error = SDL_GetError();
  if (error && error[0] != '\0') {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL Error after display: %s", error);
    SDL_ClearError();
  }
}

/**
 * @brief Clear the screen with specified color
 *
 * This method:
 * 1. Sets the renderer's draw color to the specified color
 * 2. Clears the entire screen with that color
 *
 * @param color The color to clear the screen with (RGBA)
 */
void GameEngine::clear(const GameColor &color) {
  SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
  SDL_RenderClear(renderer);
}

/**
 * @brief Present the rendered frame to the screen
 *
 * This method:
 * 1. Updates the window with the current renderer state
 * 2. Swaps the front and back buffers
 * 3. Makes all rendered content visible
 *
 * This is the final step in the rendering process and must be called
 * after all rendering operations are complete.
 */
void GameEngine::present() { SDL_RenderPresent(renderer); }

/**
 * @brief Stops the game loop
 * Sets running to false, which will exit the game loop
 */
void GameEngine::quit() { running = false; }

/**
 * @brief Clean up SDL resources
 * Called by destructor to ensure proper cleanup
 */
void GameEngine::destroy() {
#ifdef USE_SDL3_TTF
  if (font) {
    TTF_CloseFont(font);
    font = nullptr;
  }
  if (textSurface) {
    SDL_DestroySurface(textSurface);
    textSurface = nullptr;
  }
  if (textTexture) {
    SDL_DestroyTexture(textTexture);
    textTexture = nullptr;
  }
  TTF_Quit();
#endif
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

#ifdef USE_SDL3_TTF
void GameEngine::renderText(const std::string &text, int x, int y) {
  // Create surface from text
  SDL_Color textColor = {255, 255, 255, 255}; // White
  if (textSurface) {
    SDL_DestroySurface(textSurface);
  }
  textSurface =
      TTF_RenderText_Solid(font, text.c_str(), text.length(), textColor);
  if (!textSurface) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Surface Creation Error: %s",
                 SDL_GetError());
    return;
  }

  // Create texture from surface
  if (textTexture) {
    SDL_DestroyTexture(textTexture);
  }
  textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
  if (!textTexture) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Texture Creation Error: %s",
                 SDL_GetError());
    return;
  }

  // Render the texture
  SDL_FRect destRect = {static_cast<float>(x), static_cast<float>(y),
                        static_cast<float>(textSurface->w),
                        static_cast<float>(textSurface->h)};
  SDL_RenderTexture(renderer, textTexture, nullptr, &destRect);
}
#endif
} // namespace game