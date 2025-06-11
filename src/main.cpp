#include "game/GameEngine.hpp"
#include <iostream>

int main(int argc, char *argv[]) {
  try {
    game::GameEngine engine(800, 600, "Game Engine Window");

    if (!engine.init()) {
      std::cerr << "Failed to initialize game engine" << std::endl;
      return 1;
    }

    std::cout << "Starting game loop..." << std::endl;
    engine.run();
    std::cout << "Game loop ended" << std::endl;

  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }

  return 0;
}