#include "GameHUD.hpp"
#include "../ecs/components/ShootingGalleryState.hpp"
#include <format>
#include <sstream>
#include <iomanip>
#include <SDL3/SDL.h>

namespace game {
namespace ui {

GameHUD::GameHUD(int screenWidth, int screenHeight)
    : screenWidth(screenWidth)
    , screenHeight(screenHeight)
    , textRenderer(std::make_unique<TextRenderer>())
    , gameState(nullptr)
    , visible(true)
    , normalColor(255, 255, 255)   // White
    , warningColor(255, 255, 0)    // Yellow
    , criticalColor(255, 0, 0)     // Red
    , goodColor(0, 255, 0)         // Green
{
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "GameHUD initialized");
}

GameHUD::~GameHUD() {
    cleanup();
}

void GameHUD::update(float deltaTime) {
    // Get the game state instance
    if (gameState == nullptr) {
        try {
            gameState = &ecs::components::ShootingGalleryState::getInstance();
        } catch (const std::exception& e) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "[GameHUD] Failed to get ShootingGalleryState instance: %s", e.what());
            return;
        }
    }
}

void GameHUD::render(SDL_Renderer* renderer) {
    if (!visible) {
        return;
    }
    
    if (gameState == nullptr) {
        return;
    }
    
    // Render different HUD elements based on game state
    if (gameState->isPlaying()) {
        renderGameplayHUD(renderer);
    } else if (gameState->isGameOver()) {
        renderGameOverHUD(renderer);
    } else if (gameState->isMenu()) {
        renderMenuHUD(renderer);
    }
}

void GameHUD::renderGameplayHUD(SDL_Renderer* renderer) {
    // Score (top left)
    std::ostringstream scoreStream;
    scoreStream.imbue(std::locale::classic()); // Use classic locale to avoid locale errors
    scoreStream << "Score: " << gameState->score;
    std::string scoreText = scoreStream.str();
    textRenderer->renderText(renderer, scoreText, 20, 20, 32, &normalColor);
    
    // High Score (top left, below score)
    std::ostringstream highScoreStream;
    highScoreStream.imbue(std::locale::classic());
    highScoreStream << "High Score: " << gameState->highScore;
    std::string highScoreText = highScoreStream.str();
    textRenderer->renderText(renderer, highScoreText, 20, 60, 24, &goodColor);
    
    // Time remaining (top center)
    GameColor timeColor = getTimeColor(gameState->timeRemaining);
    std::ostringstream timeStream;
    timeStream << std::fixed << std::setprecision(1) << "Time: " << gameState->timeRemaining << "s";
    std::string timeText = timeStream.str();
    int timeX = (screenWidth - textRenderer->getTextWidth(timeText, 36)) / 2;
    textRenderer->renderText(renderer, timeText, timeX, 20, 36, &timeColor);
    
    // Shots fired and accuracy (top right)
    std::string shotsText = "Shots: " + std::to_string(gameState->shotsFired);
    int shotsWidth = textRenderer->getTextWidth(shotsText, 24);
    textRenderer->renderText(renderer, shotsText, screenWidth - shotsWidth - 20, 20, 24, &normalColor);
    
    std::ostringstream accuracyStream;
    accuracyStream << std::fixed << std::setprecision(1) << "Accuracy: " << gameState->getAccuracy() << "%";
    std::string accuracyText = accuracyStream.str();
    int accuracyWidth = textRenderer->getTextWidth(accuracyText, 24);
    GameColor accuracyColor = getAccuracyColor(gameState->getAccuracy());
    textRenderer->renderText(renderer, accuracyText, screenWidth - accuracyWidth - 20, 50, 24, &accuracyColor);
    
    // Targets hit (top right, below accuracy)
    std::string hitsText = "Hits: " + std::to_string(gameState->targetsHit);
    int hitsWidth = textRenderer->getTextWidth(hitsText, 24);
    textRenderer->renderText(renderer, hitsText, screenWidth - hitsWidth - 20, 80, 24, &normalColor);
}

void GameHUD::renderGameOverHUD(SDL_Renderer* renderer) {
    // Game Over title (center top)
    std::string gameOverText = "GAME OVER!";
    int goX = (screenWidth - textRenderer->getTextWidth(gameOverText, 72)) / 2;
    textRenderer->renderText(renderer, gameOverText, goX, 150, 72, &criticalColor);
    
    // Final Score (center)
    std::ostringstream finalScoreStream;
    finalScoreStream.imbue(std::locale::classic());
    finalScoreStream << "Final Score: " << gameState->score;
    std::string finalScoreText = finalScoreStream.str();
    int fsX = (screenWidth - textRenderer->getTextWidth(finalScoreText, 48)) / 2;
    textRenderer->renderText(renderer, finalScoreText, fsX, 250, 48, &normalColor);
    
    // High Score (center, below final score)
    std::ostringstream highScoreStream;
    highScoreStream.imbue(std::locale::classic());
    highScoreStream << "High Score: " << gameState->highScore;
    std::string highScoreText = highScoreStream.str();
    int hsX = (screenWidth - textRenderer->getTextWidth(highScoreText, 36)) / 2;
    GameColor hsColor = gameState->score >= gameState->highScore ? goodColor : normalColor;
    textRenderer->renderText(renderer, highScoreText, hsX, 310, 36, &hsColor);
    
    // Statistics
    int statsY = 380;
    std::vector<std::string> stats = {
        "Shots Fired: " + std::to_string(gameState->shotsFired),
        "Targets Hit: " + std::to_string(gameState->targetsHit)
    };
    
    // Add accuracy with proper formatting
    std::ostringstream accuracyStream;
    accuracyStream << std::fixed << std::setprecision(1) << "Accuracy: " << gameState->getAccuracy() << "%";
    stats.push_back(accuracyStream.str());
    
    for (const std::string& stat : stats) {
        int statX = (screenWidth - textRenderer->getTextWidth(stat, 28)) / 2;
        textRenderer->renderText(renderer, stat, statX, statsY, 28, &normalColor);
        statsY += 35;
    }
    
    // Restart instruction
    std::string restartText = "Press Q to quit";
    int restartX = (screenWidth - textRenderer->getTextWidth(restartText, 24)) / 2;
    textRenderer->renderText(renderer, restartText, restartX, 520, 24, &warningColor);
}

void GameHUD::renderMenuHUD(SDL_Renderer* renderer) {
    // Title
    std::string titleText = "DUCK SHOOTER";
    int titleX = (screenWidth - textRenderer->getTextWidth(titleText, 64)) / 2;
    textRenderer->renderText(renderer, titleText, titleX, 200, 64, &normalColor);
    
    // High Score
    std::ostringstream highScoreStream;
    highScoreStream.imbue(std::locale::classic());
    highScoreStream << "High Score: " << gameState->highScore;
    std::string highScoreText = highScoreStream.str();
    int hsX = (screenWidth - textRenderer->getTextWidth(highScoreText, 32)) / 2;
    textRenderer->renderText(renderer, highScoreText, hsX, 300, 32, &goodColor);
    
    // Instructions
    std::vector<std::string> instructions = {
        "Use A and D to move left and right",
        "Press SPACE to shoot",
        "Hit the ducks to score points!",
        "",
        "Press SPACE to start game"
    };
    
    int instY = 380;
    for (const std::string& instruction : instructions) {
        if (!instruction.empty()) {  // Skip empty lines
            int instX = (screenWidth - textRenderer->getTextWidth(instruction, 24)) / 2;
            textRenderer->renderText(renderer, instruction, instX, instY, 24, &normalColor);
        }
        instY += 30;
    }
}

GameColor GameHUD::getTimeColor(float timeRemaining) {
    if (timeRemaining <= 10.0f) {
        return criticalColor;  // Red for last 10 seconds
    } else if (timeRemaining <= 20.0f) {
        return warningColor;   // Yellow for last 20 seconds
    } else {
        return normalColor;    // White for normal time
    }
}

GameColor GameHUD::getAccuracyColor(float accuracy) {
    if (accuracy >= 80.0f) {
        return goodColor;      // Green for excellent accuracy
    } else if (accuracy >= 60.0f) {
        return normalColor;    // White for good accuracy
    } else if (accuracy >= 40.0f) {
        return warningColor;   // Yellow for mediocre accuracy
    } else {
        return criticalColor;  // Red for poor accuracy
    }
}

void GameHUD::toggleVisibility() {
    visible = !visible;
}

void GameHUD::show() {
    visible = true;
}

void GameHUD::hide() {
    visible = false;
}

bool GameHUD::isVisible() const {
    return visible;
}

TextRenderer& GameHUD::getTextRenderer() {
    return *textRenderer;
}

void GameHUD::cleanup() {
    if (textRenderer) {
        textRenderer->clearCache();
    }
}

} // namespace ui
} // namespace game 