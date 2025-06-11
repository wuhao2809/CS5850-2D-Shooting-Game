#include "game/GameEngine.hpp"
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <filesystem>
#include <iostream>
#include <sstream>

namespace fs = std::filesystem;
using namespace game;

std::string findLessonRoot(const char *argv0) {
  fs::path currentDir = fs::absolute(fs::path(argv0)).parent_path();
  while (currentDir.has_parent_path()) {
    if (fs::exists(currentDir / "GameAssets")) {
      return currentDir.string();
    }
    currentDir = currentDir.parent_path();
  }
  throw std::runtime_error("Could not find GameAssets directory");
}

void verifyAssetsDirectory(const fs::path &assetsDir) {
  if (!fs::is_directory(assetsDir)) {
    std::stringstream error;
    error
        << "GameAssets directory not found at: " << assetsDir.string() << "\n"
        << "Please ensure the GameAssets directory exists in the lesson root.";
    throw std::runtime_error(error.str());
  }
}

int main(int argc, char *argv[]) {
  try {
    std::cout << "Starting game engine initialization..." << std::endl;

    // Window configuration
    const int windowWidth = 800;
    const int windowHeight = 600;
    const std::string windowTitle = "C++ Game: Event System";

    std::cout << "Finding lesson root directory..." << std::endl;
    // Find and verify assets directory
    std::string lessonRoot = findLessonRoot(argv[0]);
    std::cout << "Lesson root found at: " << lessonRoot << std::endl;

    fs::path assetsDir = fs::path(lessonRoot) / "GameAssets";
    std::cout << "Checking assets directory at: " << assetsDir.string()
              << std::endl;
    verifyAssetsDirectory(assetsDir);
    std::cout << "Assets directory verified successfully" << std::endl;

    std::cout << "Creating game engine instance..." << std::endl;
    GameEngine engine(windowTitle, assetsDir.string());

    std::cout << "Initializing game engine..." << std::endl;
    if (!engine.init()) {
      throw std::runtime_error("Failed to initialize game engine");
    }

    std::cout << "Starting game loop..." << std::endl;
    engine.run();
    return 0;
  } catch (const std::exception &e) {
    std::cerr << "Fatal Error: " << e.what() << std::endl;
    std::cerr << "SDL Error: " << SDL_GetError() << std::endl;
    return 1;
  }
}