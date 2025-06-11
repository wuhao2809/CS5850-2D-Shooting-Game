#pragma once

#include "GameColor.hpp"
#include <SDL3/SDL.h>
#include <memory>
#include <string>

namespace game {
class GameEngine {
public:
  GameEngine(int width = 800, int height = 600,
             const std::string &title = "Game Engine");
  ~GameEngine();

  bool init();
  void run();
  void quit();

private:
  SDL_Window *window;
  SDL_Renderer *renderer;
  int width;
  int height;
  std::string title;
  bool running;
  GameColor backgroundColor; // Background color

  void handleEvents();
  void update();
  void display();
  void clear(const GameColor &color);
  void present();
  void destroy();
};
} // namespace game