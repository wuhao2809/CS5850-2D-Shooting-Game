#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <tuple>
#include <vector>
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include "../GameColor.hpp"

namespace game {
namespace ui {

/**
 * TextRenderer - A utility class for rendering text with caching and formatting.
 * This provides the core text rendering functionality used by UI components.
 * 
 * Based on: Lesson-40-WorldState/Python/src/game/ui/text_renderer.py
 *           Lesson-40-WorldState/Java/src/game/ui/TextRenderer.java
 */
class TextRenderer {
public:
    /**
     * Initialize the text renderer.
     */
    TextRenderer();
    
    /**
     * Destructor - cleanup fonts and resources
     */
    ~TextRenderer();
    
    /**
     * Get a font from the cache or create a new one.
     * 
     * @param size Font size
     * @return The requested font
     */
    TTF_Font* getFont(int size);
    
    /**
     * Render text to a renderer.
     * 
     * @param renderer SDL renderer to render to
     * @param text Text to render
     * @param x X coordinate
     * @param y Y coordinate
     * @param fontSize Font size (uses default if 0)
     * @param color RGB color (uses default if nullptr)
     * @param centerX If true, center the text horizontally around x
     * @param centerY If true, center the text vertically around y
     * @return Rectangle representing the rendered text bounds
     */
    SDL_FRect renderText(SDL_Renderer* renderer, const std::string& text, int x, int y,
                        int fontSize = 0, const GameColor* color = nullptr,
                        bool centerX = false, bool centerY = false);
    
    /**
     * Render text with default font size and color.
     * 
     * @param renderer SDL renderer to render to
     * @param text Text to render
     * @param x X coordinate
     * @param y Y coordinate
     * @return Rectangle representing the rendered text bounds
     */
    SDL_FRect renderText(SDL_Renderer* renderer, const std::string& text, int x, int y);
    
    /**
     * Render text centered at the given coordinates.
     * 
     * @param renderer SDL renderer to render to
     * @param text Text to render
     * @param centerX Center X coordinate
     * @param centerY Center Y coordinate
     * @param fontSize Font size (uses default if 0)
     * @param color RGB color (uses default if nullptr)
     * @return Rectangle representing the rendered text bounds
     */
    SDL_FRect renderTextCentered(SDL_Renderer* renderer, const std::string& text, 
                                int centerX, int centerY, int fontSize = 0, 
                                const GameColor* color = nullptr);
    
    /**
     * Get the size of text when rendered.
     * 
     * @param text Text to measure
     * @param fontSize Font size (uses default if 0)
     * @return Tuple of (width, height)
     */
    std::tuple<int, int> getTextSize(const std::string& text, int fontSize = 0);
    
    /**
     * Get the width of text when rendered.
     * 
     * @param text Text to measure
     * @param fontSize Font size (uses default if 0)
     * @return Width in pixels
     */
    int getTextWidth(const std::string& text, int fontSize = 0);
    
    /**
     * Get the height of text when rendered.
     * 
     * @param text Text to measure
     * @param fontSize Font size (uses default if 0)
     * @return Height in pixels
     */
    int getTextHeight(const std::string& text, int fontSize = 0);
    
    /**
     * Render multiple lines of text.
     * 
     * @param renderer SDL renderer to render to
     * @param lines Vector of text lines to render
     * @param x X coordinate
     * @param y Y coordinate of first line
     * @param fontSize Font size (uses default if 0)
     * @param color RGB color (uses default if nullptr)
     * @param lineSpacing Additional spacing between lines
     */
    void renderMultilineText(SDL_Renderer* renderer, const std::vector<std::string>& lines,
                           int x, int y, int fontSize = 0, const GameColor* color = nullptr,
                           int lineSpacing = 5);
    
    /**
     * Clear the font cache.
     */
    void clearCache();
    
    /**
     * Get the number of fonts in the cache.
     * This method is primarily for testing purposes.
     * 
     * @return Number of fonts in the cache
     */
    size_t getFontCacheSize() const;

private:
    // Font cache to improve performance
    std::unordered_map<int, std::unique_ptr<TTF_Font, decltype(&TTF_CloseFont)>> fontCache;
    
    // Default settings
    int defaultFontSize;
    GameColor defaultColor;
    
    /**
     * Create a texture from text
     * 
     * @param renderer SDL renderer
     * @param text Text to render
     * @param font Font to use
     * @param color Color to use
     * @return Created texture (caller owns)
     */
    SDL_Texture* createTextTexture(SDL_Renderer* renderer, const std::string& text, 
                                  TTF_Font* font, const GameColor& color);
};

} // namespace ui
} // namespace game 