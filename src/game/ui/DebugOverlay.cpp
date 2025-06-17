#include "DebugOverlay.hpp"
#include "../events/EventManager.hpp"
#include "../events/KeyboardEvent.hpp"
#include "../ecs/components/Collision.hpp"
#include <SDL3/SDL.h>
#include <algorithm>
#include <sstream>
#include <iomanip>

namespace game {
namespace ui {

DebugOverlay::DebugOverlay(int screenWidth, int screenHeight, Timer* timer)
    : screenWidth(screenWidth)
    , screenHeight(screenHeight)
    , timer(timer)
    , visible(false)
    , collisionInfoVisible(false)
    , performanceVisible(false)
    , entityInfoVisible(false)
    , textRenderer(std::make_unique<TextRenderer>())
    , primaryTextColor(0, 0, 0)        // Black
    , secondaryTextColor(0, 0, 0)      // Black
    , warningColor(255, 165, 0)        // Orange
    , errorColor(255, 69, 0)           // Red-orange
    , successColor(0, 0, 0)            // Black for good performance
    , backgroundColor(173, 216, 230, 190) // Light blue with alpha
{
    // No local performance tracking needed - Timer provides FPS
    
    // Subscribe to keyboard events
    events::EventManager::getInstance().subscribe("keyboard", this);
    
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "DebugOverlay initialized");
}

DebugOverlay::~DebugOverlay() {
    // Unsubscribe from keyboard events
    events::EventManager::getInstance().unsubscribe("keyboard", this);
}

void DebugOverlay::onEvent(const events::Event& event) {
    if (event.getType() == "keyboard") {
        const events::KeyboardEvent& keyEvent = static_cast<const events::KeyboardEvent&>(event);
        if (keyEvent.isPressed()) {
            std::string key = keyEvent.getKeyText();
            
            if (key == "f1") {
                toggleVisibility();
                SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Debug overlay visibility toggled to: %s", visible ? "true" : "false");
            }
            else if (key == "f2") {
                toggleCollisionInfo();
                SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Collision info visibility toggled to: %s", collisionInfoVisible ? "true" : "false");
            }
            else if (key == "f3") {
                togglePerformanceInfo();
                SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Performance info visibility toggled to: %s", performanceVisible ? "true" : "false");
            }
            else if (key == "f4") {
                toggleEntityInfo();
                SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Entity info visibility toggled to: %s", entityInfoVisible ? "true" : "false");
            }
        }
    }
}

void DebugOverlay::update(float deltaTime) {
    // No local tracking needed - Timer provides all performance data
}

void DebugOverlay::render(SDL_Renderer* renderer) {
    if (!visible) {
        return;
    }
    
    // Draw semi-transparent background
    drawBackground(renderer);
    
    // Draw debug help
    renderDebugHelp(renderer);
    
    // Draw performance info if enabled
    if (performanceVisible) {
        renderPerformanceInfo(renderer);
    }
    
    // Draw entity info if enabled
    if (entityInfoVisible) {
        renderEntityInfo(renderer);
    }
}

void DebugOverlay::renderCollisionInfo(SDL_Renderer* renderer,
                                     const std::unordered_map<std::string, ecs::components::Collision*>& collisionComponents) {
    if (!visible || !collisionInfoVisible) {
        return;
    }
    
    int yOffset = 200;
    
    // Header
    textRenderer->renderText(renderer, "=== COLLISION DEBUG ===", 10, yOffset, 24, &secondaryTextColor);
    yOffset += 30;
    
    // Count total collision components (entities capable of collision)
    int totalCollisionEntities = 0;
    for (const auto& pair : collisionComponents) {
        if (pair.second) {
            totalCollisionEntities++;
        }
    }
    
    std::string countText = "Entities with Collision: " + std::to_string(totalCollisionEntities);
    textRenderer->renderText(renderer, countText, 10, yOffset, 20, &primaryTextColor);
    yOffset += 25;
    
    // Show details for each entity with collision component
    for (const auto& pair : collisionComponents) {
        const std::string& entityId = pair.first;
        const ecs::components::Collision* collision = pair.second;
        
        if (collision) {
            std::string collisionText = "Entity " + entityId + ": " + collision->getType() + " collision";
            textRenderer->renderText(renderer, collisionText, 20, yOffset, 18, &primaryTextColor);
            yOffset += 20;
        }
    }
}

void DebugOverlay::toggleVisibility() {
    visible = !visible;
}

void DebugOverlay::setVisible(bool newVisible) {
    visible = newVisible;
}

bool DebugOverlay::isVisible() const {
    return visible;
}

void DebugOverlay::toggleCollisionInfo() {
    collisionInfoVisible = !collisionInfoVisible;
}

void DebugOverlay::togglePerformanceInfo() {
    performanceVisible = !performanceVisible;
}

void DebugOverlay::toggleEntityInfo() {
    entityInfoVisible = !entityInfoVisible;
}

TextRenderer& DebugOverlay::getTextRenderer() {
    return *textRenderer;
}

void DebugOverlay::drawBackground(SDL_Renderer* renderer) {
    // Set blend mode for alpha transparency
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    
    // Draw semi-transparent background
    SDL_SetRenderDrawColor(renderer, backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
    SDL_FRect backgroundRect = {0.0f, 0.0f, static_cast<float>(screenWidth), static_cast<float>(screenHeight)};
    SDL_RenderFillRect(renderer, &backgroundRect);
    
    // Reset blend mode
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
}

void DebugOverlay::renderDebugHelp(SDL_Renderer* renderer) {
    std::vector<std::string> helpText = {
        "=== DEBUG CONTROLS ===",
        "F1: Toggle Debug Overlay",
        "F2: Toggle Collision Info",
        "F3: Toggle Performance Info",
        "F4: Toggle Entity Info"
    };
    
    int yOffset = 10;
    for (const std::string& line : helpText) {
        GameColor* color = line.find("===") != std::string::npos ? &secondaryTextColor : &primaryTextColor;
        int fontSize = line.find("===") != std::string::npos ? 22 : 16;
        textRenderer->renderText(renderer, line, 10, yOffset, fontSize, color);
        yOffset += 20;
    }
}

void DebugOverlay::renderPerformanceInfo(SDL_Renderer* renderer) {
    // Get FPS from Timer (authoritative source)
    int fps = timer->getFps();
    
    // Position performance info on the right side
    int xPos = screenWidth - 250;
    int yOffset = 10;
    
    // Header
    textRenderer->renderText(renderer, "=== PERFORMANCE ===", xPos, yOffset, 22, &secondaryTextColor);
    yOffset += 30;
    
    // FPS with color coding
    GameColor* fpsColor;
    if (fps < 30) {
        fpsColor = &errorColor;
    } else if (fps < 50) {
        fpsColor = &warningColor;
    } else {
        fpsColor = &successColor;
    }
    
    std::string fpsText = "FPS: " + std::to_string(fps);
    textRenderer->renderText(renderer, fpsText, xPos, yOffset, 18, fpsColor);
}

void DebugOverlay::renderEntityInfo(SDL_Renderer* renderer) {
    // Position entity info on the right side, below performance
    int xPos = screenWidth - 250;
    int yOffset = 150;
    
    std::vector<std::string> entityText = {
        "=== ENTITY INFO ===",
        "Total Entities: N/A",
        "Active Systems: N/A",
        "Components: N/A"
    };
    
    for (const std::string& line : entityText) {
        GameColor* color = line.find("===") != std::string::npos ? &secondaryTextColor : &primaryTextColor;
        int fontSize = line.find("===") != std::string::npos ? 22 : 16;
        textRenderer->renderText(renderer, line, xPos, yOffset, fontSize, color);
        yOffset += 20;
    }
}

} // namespace ui
} // namespace game 