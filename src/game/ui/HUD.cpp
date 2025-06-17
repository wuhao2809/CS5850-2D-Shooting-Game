#include "HUD.hpp"
#include <stdexcept>
#include <iostream>
#include "../events/KeyboardEvent.hpp"
#include "../events/EventManager.hpp"
#include "../ecs/components/Collision.hpp"
#include <SDL3/SDL.h>
#include "../GameColor.hpp"

namespace game {

HUD::HUD(int screenWidth, int screenHeight, Timer* timer, bool enableDebug)
    : screenWidth(screenWidth)
    , screenHeight(screenHeight)
    , enableDebug(enableDebug)
    , visible(true)
    , gameHUD(std::make_unique<ui::GameHUD>(screenWidth, screenHeight))
    , debugOverlay(enableDebug ? std::make_unique<ui::DebugOverlay>(screenWidth, screenHeight, timer) : nullptr)
{
    // Subscribe to keyboard events for global HUD controls
    events::EventManager::getInstance().subscribe("keyboard", this);
    
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "HUD Master Coordinator initialized (debug=%s)", 
                enableDebug ? "enabled" : "disabled");
}

HUD::~HUD() {
    // Unsubscribe from keyboard events
    events::EventManager::getInstance().unsubscribe("keyboard", this);
}

void HUD::onEvent(const events::Event& event) {
    if (event.getType() == "keyboard") {
        const events::KeyboardEvent& keyEvent = static_cast<const events::KeyboardEvent&>(event);
        if (keyEvent.getKeyText() == "h" && keyEvent.isPressed()) {
            toggleGameHUD();
            SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Game HUD visibility toggled to: %s", gameHUD->isVisible() ? "true" : "false");
        }
    }
}

void HUD::update(float deltaTime) {
    // Update game HUD (always render first, so debug info appears on top)
    if (gameHUD) {
        gameHUD->update(deltaTime);
    }
    
    // Update debug overlay if enabled
    if (debugOverlay) {
        debugOverlay->update(deltaTime);
    }
}

void HUD::render(SDL_Renderer* renderer) {
    if (!visible) {
        return;
    }
    
    // Render game HUD (always render first, so debug info appears on top)
    if (gameHUD) {
        gameHUD->render(renderer);
    }
    
    // Render debug overlay if enabled
    if (debugOverlay) {
        debugOverlay->render(renderer);
    }
}

void HUD::renderCollisionDebug(SDL_Renderer* renderer, 
                              const std::unordered_map<std::string, ecs::components::Collision*>& collisionComponents) {
    if (!visible) {
        return;
    }
    
    // Store collision components for debug rendering
    currentCollisions = collisionComponents;
    
    // Forward to debug overlay if enabled
    if (debugOverlay) {
        debugOverlay->renderCollisionInfo(renderer, collisionComponents);
    }
}

void HUD::toggleVisibility() {
    visible = !visible;
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "HUD Master visibility toggled to: %s", visible ? "true" : "false");
}

void HUD::setVisible(bool newVisible) {
    visible = newVisible;
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "HUD Master visibility set to: %s", visible ? "true" : "false");
}

bool HUD::isVisible() const {
    return visible;
}

ui::GameHUD& HUD::getGameHUD() {
    return *gameHUD;
}

ui::DebugOverlay* HUD::getDebugOverlay() {
    return debugOverlay.get();
}

bool HUD::isDebugEnabled() const {
    return enableDebug;
}

// Convenience methods for controlling HUD components

void HUD::showGameHUD() {
    if (gameHUD) {
        gameHUD->show();
    }
}

void HUD::hideGameHUD() {
    if (gameHUD) {
        gameHUD->hide();
    }
}

void HUD::toggleGameHUD() {
    if (gameHUD) {
        gameHUD->toggleVisibility();
    }
}

void HUD::showDebug() {
    if (debugOverlay) {
        debugOverlay->setVisible(true);
    }
}

void HUD::hideDebug() {
    if (debugOverlay) {
        debugOverlay->setVisible(false);
    }
}

void HUD::toggleDebug() {
    if (debugOverlay) {
        debugOverlay->toggleVisibility();
    }
}

} // namespace game 