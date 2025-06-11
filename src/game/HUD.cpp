#include "HUD.hpp"
#include <stdexcept>
#include <iostream>
#include "events/KeyboardEvent.hpp"
#include "events/EventManager.hpp"
#include <SDL3/SDL.h>
#include "GameColor.hpp"

namespace game {

HUD::HUD(int screenWidth, int screenHeight)
    : screenWidth(screenWidth)
    , screenHeight(screenHeight)
    , visible(true)
{
    // Initialize SDL_ttf if not already initialized
    if (!TTF_WasInit()) {
        if (!TTF_Init() ) {
            throw std::runtime_error("Failed to initialize SDL_ttf: " + std::string(SDL_GetError()));
        }
    }

    // Subscribe to keyboard events
    events::EventManager::getInstance().subscribe("keyboard", this);
}

HUD::~HUD() {
    // Unsubscribe from keyboard events
    events::EventManager::getInstance().unsubscribe("keyboard", this);
    clear();
}

void HUD::onEvent(const events::Event& event) {
    if (event.getType() == "keyboard") {
        const events::KeyboardEvent& keyEvent = static_cast<const events::KeyboardEvent&>(event);
        if (keyEvent.getKeyText() == "h" && keyEvent.isPressed()) {
            visible = !visible;
        }
    }
}

void HUD::addText(const std::string& id, const std::string& text, int x, int y) {
    addText(id, text, x, y, 20); // Default font size
}

void HUD::addText(const std::string& id, const std::string& text, int x, int y, int fontSize) {
    if (textElements.find(id) != textElements.end()) {
        throw std::invalid_argument("Text element with id '" + id + "' already exists");
    }
    
    // Ensure font is cached
    getFont(fontSize);
    
    textElements.emplace(id, TextElement(text, x, y, fontSize, GameColor(255, 255, 255)));
}

void HUD::updateText(const std::string& id, const std::string& newText) {
    auto it = textElements.find(id);
    if (it == textElements.end()) {
        throw std::invalid_argument("Text element with id '" + id + "' not found");
    }
    
    it->second.text = newText;
    it->second.texture = nullptr; // Force texture update on next render
}

void HUD::removeText(const std::string& id) {
    auto it = textElements.find(id);
    if (it == textElements.end()) {
        throw std::invalid_argument("Text element with id '" + id + "' not found");
    }
    
    if (it->second.texture) {
        SDL_DestroyTexture(it->second.texture);
    }
    textElements.erase(it);
}

void HUD::setPosition(const std::string& id, int x, int y) {
    auto it = textElements.find(id);
    if (it == textElements.end()) {
        throw std::invalid_argument("Text element with id '" + id + "' not found");
    }
    
    it->second.x = x;
    it->second.y = y;
}

void HUD::setFontSize(const std::string& id, int size) {
    auto it = textElements.find(id);
    if (it == textElements.end()) {
        throw std::invalid_argument("Text element with id '" + id + "' not found");
    }
    
    // Ensure font is cached
    getFont(size);
    
    if (it->second.texture) {
        SDL_DestroyTexture(it->second.texture);
        it->second.texture = nullptr;
    }
    it->second.fontSize = size;
}

void HUD::setColor(const std::string& id, const GameColor& color) {
    auto it = textElements.find(id);
    if (it == textElements.end()) {
        throw std::invalid_argument("Text element with id '" + id + "' not found");
    }
    
    if (it->second.texture) {
        SDL_DestroyTexture(it->second.texture);
        it->second.texture = nullptr;
    }
    it->second.color = color;
}

void HUD::toggleVisibility() {
    visible = !visible;
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "HUD visibility toggled to: %s", visible ? "true" : "false");
}

void HUD::setVisible(bool visible) {
    this->visible = visible;
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "HUD visibility set to: %s", visible ? "true" : "false");
}

bool HUD::isVisible() const {
    return visible;
}

void HUD::render(SDL_Renderer* renderer) {
    if (!visible) {
        SDL_LogDebug(SDL_LOG_CATEGORY_RENDER, "HUD is not visible");
        return;
    }
    
    SDL_LogDebug(SDL_LOG_CATEGORY_RENDER, "Rendering %zu text elements", textElements.size());
    for (auto& pair : textElements) {
        TextElement& element = pair.second;
        
        // Update texture if needed
        if (!element.texture) {
            updateTexture(element, renderer);
        }
        
        if (element.texture) {
            float w, h;
            SDL_GetTextureSize(element.texture, &w, &h);
            SDL_FRect dest = {static_cast<float>(element.x), static_cast<float>(element.y), w, h};
            SDL_RenderTexture(renderer, element.texture, nullptr, &dest);
            SDL_LogDebug(SDL_LOG_CATEGORY_RENDER, "Drawing text: %s at (%d, %d)", element.text.c_str(), element.x, element.y);
        }
    }
}

void HUD::clear() {
    for (auto& pair : textElements) {
        if (pair.second.texture) {
            SDL_DestroyTexture(pair.second.texture);
        }
    }
    textElements.clear();
}

size_t HUD::getFontCacheSize() const {
    return fontCache.size();
}

TTF_Font* HUD::getFont(int size) {
    auto it = fontCache.find(size);
    if (it == fontCache.end()) {
#ifdef __APPLE__
        TTF_Font* font = TTF_OpenFont("/System/Library/Fonts/Helvetica.ttc", size);
#else
        TTF_Font* font = TTF_OpenFont("C:\\Windows\\Fonts\\arial.ttf", size);
#endif
        if (!font) {
            throw std::runtime_error("Failed to load font: " + std::string(SDL_GetError()));
        }
        fontCache.emplace(size, std::unique_ptr<TTF_Font, decltype(&TTF_CloseFont)>(font, TTF_CloseFont));
        return font;
    }
    return it->second.get();
}

void HUD::updateTexture(TextElement& element, SDL_Renderer* renderer) {
    if (element.texture) {
        SDL_DestroyTexture(element.texture);
    }
    
    TTF_Font* font = getFont(element.fontSize);
    SDL_Color color = {element.color.r, element.color.g, element.color.b, element.color.a};
    
    SDL_Surface* surface = TTF_RenderText_Solid(font, element.text.c_str(), element.text.length(), color);
    if (!surface) {
        throw std::runtime_error("Failed to render text: " + std::string(SDL_GetError()));
    }
    
    element.texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_DestroySurface(surface);
    
    if (!element.texture) {
        throw std::runtime_error("Failed to create texture: " + std::string(SDL_GetError()));
    }
}

} // namespace game 