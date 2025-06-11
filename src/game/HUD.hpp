#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include "GameColor.hpp"
#include "events/EventListener.hpp"

namespace game {

/**
 * HUD class for managing and rendering text elements on the screen.
 * Supports text positioning, font size, color, and fade effects.
 */
class HUD : public events::EventListener {
public:
    /**
     * Constructor for HUD
     * 
     * @param screenWidth Width of the screen
     * @param screenHeight Height of the screen
     */
    HUD(int screenWidth, int screenHeight);
    
    /**
     * Destructor
     */
    ~HUD();
    
    /**
     * Handle incoming events
     * Implements EventListener interface
     * 
     * @param event The event to handle
     */
    void onEvent(const events::Event& event) override;
    
    /**
     * Add a text element to the HUD
     * 
     * @param id Unique identifier for the text element
     * @param text Text to display
     * @param x X coordinate
     * @param y Y coordinate
     * @throws std::invalid_argument if a text element with the given ID already exists
     */
    void addText(const std::string& id, const std::string& text, int x, int y);
    
    /**
     * Add a text element to the HUD with custom font size
     * 
     * @param id Unique identifier for the text element
     * @param text Text to display
     * @param x X coordinate
     * @param y Y coordinate
     * @param fontSize Font size in pixels
     * @throws std::invalid_argument if a text element with the given ID already exists
     */
    void addText(const std::string& id, const std::string& text, int x, int y, int fontSize);
    
    /**
     * Update the text of an existing element
     * 
     * @param id Identifier of the text element
     * @param newText New text to display
     * @throws std::invalid_argument if the text element is not found
     */
    void updateText(const std::string& id, const std::string& newText);
    
    /**
     * Remove a text element from the HUD
     * 
     * @param id Identifier of the text element to remove
     * @throws std::invalid_argument if the text element is not found
     */
    void removeText(const std::string& id);
    
    /**
     * Set the position of a text element
     * 
     * @param id Identifier of the text element
     * @param x New X coordinate
     * @param y New Y coordinate
     * @throws std::invalid_argument if the text element is not found
     */
    void setPosition(const std::string& id, int x, int y);
    
    /**
     * Set the font size of a text element
     * 
     * @param id Identifier of the text element
     * @param size New font size in pixels
     * @throws std::invalid_argument if the text element is not found
     */
    void setFontSize(const std::string& id, int size);
    
    /**
     * Set the color of a text element
     * 
     * @param id Identifier of the text element
     * @param color New color
     * @throws std::invalid_argument if the text element is not found
     */
    void setColor(const std::string& id, const GameColor& color);
    
    /**
     * Toggle HUD visibility
     */
    void toggleVisibility();
    
    /**
     * Set HUD visibility
     * 
     * @param visible Whether the HUD should be visible
     */
    void setVisible(bool visible);
    
    /**
     * Check if the HUD is visible
     * 
     * @return Whether the HUD is visible
     */
    bool isVisible() const;
    
    /**
     * Render all text elements
     * 
     * @param renderer SDL renderer to render to
     */
    void render(SDL_Renderer* renderer);
    
    /**
     * Clear all text elements
     */
    void clear();
    
    /**
     * Get the number of fonts in the cache
     * This method is primarily for testing purposes
     * 
     * @return Number of fonts in the cache
     */
    size_t getFontCacheSize() const;

private:
    /**
     * Inner struct to store text element properties
     */
    struct TextElement {
        std::string text;
        int x;
        int y;
        int fontSize;
        GameColor color;
        SDL_Texture* texture;
        
        TextElement(const std::string& text, int x, int y, int fontSize, const GameColor& color)
            : text(text), x(x), y(y), fontSize(fontSize), color(color), texture(nullptr) {}
    };
    
    /**
     * Get a font from the cache or create a new one
     * 
     * @param size Font size
     * @return The requested font
     */
    TTF_Font* getFont(int size);
    
    /**
     * Update the texture for a text element
     * 
     * @param element Text element to update
     * @param renderer SDL renderer to create texture with
     */
    void updateTexture(TextElement& element, SDL_Renderer* renderer);
    
    int screenWidth;
    int screenHeight;
    std::unordered_map<std::string, TextElement> textElements;
    std::unordered_map<int, std::unique_ptr<TTF_Font, decltype(&TTF_CloseFont)>> fontCache;
    bool visible;
};

} // namespace game 