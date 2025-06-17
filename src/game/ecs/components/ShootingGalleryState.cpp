#include "ShootingGalleryState.hpp"
#include "../../Timer.hpp"
#include <sstream>
#include <iomanip>
#include <fstream>
#include <filesystem>
#include <stdexcept>
#include <algorithm>
#include <nlohmann/json.hpp>
#include <SDL3/SDL.h>

namespace game::ecs::components {
    
    // Static member definition
    ShootingGalleryState* ShootingGalleryState::instance_ = nullptr;
    
    ShootingGalleryState::ShootingGalleryState(const game::ecs::Entity& entity, const Timer* timer)
        : game::ecs::Component(entity)
        , timer_(timer)
    {
        if (timer_ == nullptr) {
            throw std::invalid_argument("ShootingGalleryState requires a Timer instance for hardware-independent timing");
        }
        loadHighScore();
    }
    
    ShootingGalleryState& ShootingGalleryState::getInstance() {
        if (!instance_) {
            throw std::runtime_error("ShootingGalleryState instance not created. Call createInstance() first.");
        }
        return *instance_;
    }
    
    void ShootingGalleryState::createInstance(const game::ecs::Entity& entity, const Timer* timer) {
        if (timer == nullptr) {
            throw std::invalid_argument("Timer instance required to create ShootingGalleryState singleton");
        }
        if (instance_) {
            delete instance_;
        }
        instance_ = new ShootingGalleryState(entity, timer);
    }
    
    bool ShootingGalleryState::hasInstance() {
        return instance_ != nullptr;
    }
    
    void ShootingGalleryState::destroyInstance() {
        if (instance_) {
            delete instance_;
            instance_ = nullptr;
        }
    }
    
    void ShootingGalleryState::addScore(int points) {
        if (points > 0) {
            score += points;
            targetsHit++;
            
            // Update high score if current score exceeds it
            if (score > highScore) {
                highScore = score;
                saveHighScore();
            }
        }
    }
    
    void ShootingGalleryState::recordShot() {
        shotsFired++;
    }
    
    void ShootingGalleryState::startGame() {
        resetStats();
        timeRemaining = GAME_DURATION;
        state = GameState::PLAYING;
        
        // Initialize spawn timing using game time
        double currentTime = timer_->getClock();  // ✅ Game time
        gameStartTime = currentTime;
        lastTargetSpawn = currentTime;
    }
    
    void ShootingGalleryState::endGame() {
        state = GameState::GAME_OVER;
        
        // Ensure high score is saved
        if (score > highScore) {
            highScore = score;
            saveHighScore();
        }
    }
    
    float ShootingGalleryState::getAccuracy() const {
        if (shotsFired <= 0) {
            return 0.0f;
        }
        return (static_cast<float>(targetsHit) / static_cast<float>(shotsFired)) * 100.0f;
    }
    
    bool ShootingGalleryState::isPlaying() const {
        return state == GameState::PLAYING;
    }
    
    bool ShootingGalleryState::isGameOver() const {
        return state == GameState::GAME_OVER;
    }
    
    bool ShootingGalleryState::isMenu() const {
        return state == GameState::MENU;
    }
    
    void ShootingGalleryState::updateTimer(float deltaTime) {
        if (isPlaying() && deltaTime > 0.0f) {
            timeRemaining -= deltaTime;
            timeRemaining = std::max(0.0f, timeRemaining);
            
            // Auto-transition to game over when time runs out
            if (timeRemaining <= 0.0f) {
                endGame();
            }
        }
    }
    
    bool ShootingGalleryState::isTimeUp() const {
        return timeRemaining <= 0.0f;
    }
    
    bool ShootingGalleryState::shouldSpawnTarget() {
        if (state != GameState::PLAYING) {
            return false;
        }
        
        double currentTime = timer_->getClock();  // ✅ Game time
        double timeSinceLastSpawn = currentTime - lastTargetSpawn;
        double spawnInterval = 1.0 / DUCK_SPAWN_RATE;  // Time between spawns
        
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, 
                   "[ShootingGalleryState] shouldSpawnTarget? time_since_last=%.2f, interval=%.2f",
                   timeSinceLastSpawn, spawnInterval);
        
        if (timeSinceLastSpawn >= spawnInterval) {
            lastTargetSpawn = currentTime;
            return true;
        }
        return false;
    }
    
    void ShootingGalleryState::resetStats() {
        score = 0;
        targetsHit = 0;
        shotsFired = 0;
        timeRemaining = GAME_DURATION;
    }
    
    std::string ShootingGalleryState::getStateString() const {
        return gameStateToString(state);
    }
    
    void ShootingGalleryState::setState(GameState newState) {
        state = newState;
        
        // Handle state-specific logic
        switch (newState) {
            case GameState::PLAYING:
                startGame();
                break;
            case GameState::GAME_OVER:
                // Ensure final high score check
                if (score > highScore) {
                    highScore = score;
                    saveHighScore();
                }
                break;
            case GameState::MENU:
                // Reset to menu state, keep high score
                break;
        }
    }
    
    std::string ShootingGalleryState::toString() const {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(1);
        oss << "ShootingGalleryState{entity=" << entity_.getId()
            << ", state=" << getStateString()
            << ", score=" << score << "/" << highScore
            << ", time=" << timeRemaining << "s"
            << ", shots=" << targetsHit << "/" << shotsFired
            << ", accuracy=" << std::setprecision(1) << getAccuracy() << "%"
            << "}";
        return oss.str();
    }
    
    void ShootingGalleryState::loadHighScore() {
        try {
            const std::string filePath = getHighScoreFilePath();
            SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "[ShootingGalleryState] Attempting to load high score from: %s", filePath.c_str());
            
            if (!std::filesystem::exists(filePath)) {
                SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "[ShootingGalleryState] High score file does not exist, using default score 0");
                highScore = 0;
                return;
            }
            
            std::ifstream file(filePath);
            if (!file.is_open()) {
                SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "[ShootingGalleryState] Failed to open high score file: %s", filePath.c_str());
                highScore = 0;
                return;
            }
            
            nlohmann::json jsonData;
            file >> jsonData;
            
            if (jsonData.contains("highScore") && jsonData["highScore"].is_number_integer()) {
                highScore = std::max(0, jsonData["highScore"].get<int>());
                SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "[ShootingGalleryState] Loaded high score: %d", highScore);
            } else {
                SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "[ShootingGalleryState] No high score found in file, using default 0");
                highScore = 0;
            }
        } catch (const std::exception& e) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "[ShootingGalleryState] Error loading high score: %s", e.what());
            highScore = 0;
        }
    }
    
    void ShootingGalleryState::saveHighScore() {
        try {
            const std::string filePath = getHighScoreFilePath();
            SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "[ShootingGalleryState] Saving high score to: %s", filePath.c_str());
            
            nlohmann::json jsonData;
            jsonData["highScore"] = highScore;
            jsonData["lastUpdated"] = std::time(nullptr);
            
            std::ofstream file(filePath);
            if (!file.is_open()) {
                SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "[ShootingGalleryState] Failed to open high score file for writing: %s", filePath.c_str());
                return;
            }
            
            file << jsonData.dump(2); // Pretty print with 2-space indentation
            file.close();
            
            SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "[ShootingGalleryState] Saved high score: %d", highScore);
        } catch (const std::exception& e) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "[ShootingGalleryState] Error saving high score: %s", e.what());
        }
    }
    
    std::string ShootingGalleryState::getHighScoreFilePath() const {
        // Simple current directory approach to match Java and Python implementations
        return "high_score.json";
    }
    
    // Utility functions
    std::string gameStateToString(GameState state) {
        switch (state) {
            case GameState::MENU:
                return "menu";
            case GameState::PLAYING:
                return "playing";
            case GameState::GAME_OVER:
                return "game_over";
            default:
                return "menu"; // Default fallback
        }
    }
    
    GameState stringToGameState(const std::string& stateStr) {
        std::string lowerStr = stateStr;
        std::transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(), ::tolower);
        
        if (lowerStr == "playing") {
            return GameState::PLAYING;
        } else if (lowerStr == "game_over" || lowerStr == "gameover") {
            return GameState::GAME_OVER;
        } else if (lowerStr == "menu") {
            return GameState::MENU;
        } else {
            // Default to MENU for any unrecognized string
            return GameState::MENU;
        }
    }
    
} // namespace game::ecs::components 