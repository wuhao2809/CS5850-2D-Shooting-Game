#include "TextRenderer.hpp"
#include <stdexcept>
#include <algorithm>
#include <SDL3/SDL.h>

namespace game {
namespace ui {

TextRenderer::TextRenderer()
    : defaultFontSize(24)
    , defaultColor(255, 255, 255)  // White
{
    // Initialize SDL_ttf if not already initialized
    if (!TTF_WasInit()) {
        if (!TTF_Init()) {
            throw std::runtime_error("Failed to initialize SDL_ttf: " + std::string(SDL_GetError()));
        }
    }
    
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "TextRenderer initialized with font caching");
}

TextRenderer::~TextRenderer() {
    clearCache();
}

TTF_Font* TextRenderer::getFont(int size) {
    // Clamp size to reasonable range for caching (12px to 72px)
    int clampedSize = std::max(12, std::min(72, size));
    
    auto it = fontCache.find(clampedSize);
    if (it == fontCache.end()) {
#ifdef __APPLE__
        const char* fontPath = "/System/Library/Fonts/Helvetica.ttc";
        TTF_Font* font = TTF_OpenFont(fontPath, clampedSize);
#elif defined(_WIN32)
        const char* fontPath = "C:\\Windows\\Fonts\\arial.ttf";
        TTF_Font* font = TTF_OpenFont(fontPath, clampedSize);
#else
        // Linux - try common font paths
        const char* fontPath = "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf";
        TTF_Font* font = TTF_OpenFont(fontPath, clampedSize);
        if (!font) {
            fontPath = "/usr/share/fonts/TTF/arial.ttf";
            font = TTF_OpenFont(fontPath, clampedSize);
        }
#endif
        
        if (!font) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "[TextRenderer] Failed to load font: %s", SDL_GetError());
            throw std::runtime_error("Failed to load font: " + std::string(SDL_GetError()));
        }
        
        fontCache.emplace(clampedSize, std::unique_ptr<TTF_Font, decltype(&TTF_CloseFont)>(font, TTF_CloseFont));
        SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "Created new font with size: %d", clampedSize);
        return font;
    }
    
    return it->second.get();
}

SDL_FRect TextRenderer::renderText(SDL_Renderer* renderer, const std::string& text, int x, int y,
                                  int fontSize, const GameColor* color,
                                  bool centerX, bool centerY) {
    if (text.empty()) {
        return {static_cast<float>(x), static_cast<float>(y), 0.0f, 0.0f};
    }
    
    // Use defaults if not specified
    int actualFontSize = fontSize > 0 ? fontSize : defaultFontSize;
    const GameColor& actualColor = color ? *color : defaultColor;
    
    // Get font and create texture
    TTF_Font* font = getFont(actualFontSize);
    SDL_Texture* texture = createTextTexture(renderer, text, font, actualColor);
    
    if (!texture) {
        return {static_cast<float>(x), static_cast<float>(y), 0.0f, 0.0f};
    }
    
    // Get texture dimensions
    float textWidth, textHeight;
    SDL_GetTextureSize(texture, &textWidth, &textHeight);
    
    // Calculate position based on centering options
    float renderX = static_cast<float>(x);
    float renderY = static_cast<float>(y);
    
    if (centerX) {
        renderX = x - textWidth / 2.0f;
    }
    if (centerY) {
        renderY = y - textHeight / 2.0f;
    }
    
    // Render the texture
    SDL_FRect destRect = {renderX, renderY, textWidth, textHeight};
    SDL_RenderTexture(renderer, texture, nullptr, &destRect);
    
    // Clean up texture
    SDL_DestroyTexture(texture);
    
    return destRect;
}

SDL_FRect TextRenderer::renderText(SDL_Renderer* renderer, const std::string& text, int x, int y) {
    return renderText(renderer, text, x, y, 0, nullptr, false, false);
}

SDL_FRect TextRenderer::renderTextCentered(SDL_Renderer* renderer, const std::string& text,
                                          int centerX, int centerY, int fontSize,
                                          const GameColor* color) {
    return renderText(renderer, text, centerX, centerY, fontSize, color, true, true);
}

std::tuple<int, int> TextRenderer::getTextSize(const std::string& text, int fontSize) {
    if (text.empty()) {
        return {0, 0};
    }
    
    int actualFontSize = fontSize > 0 ? fontSize : defaultFontSize;
    TTF_Font* font = getFont(actualFontSize);
    if (!font) {
        return {0, 0};
    }
    
    int width, height;
    if (!TTF_GetStringSize(font, text.c_str(), text.length(), &width, &height)) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to get text size: %s", SDL_GetError());
        return {0, 0};
    }
    
    return {width, height};
}

int TextRenderer::getTextWidth(const std::string& text, int fontSize) {
    auto [width, height] = getTextSize(text, fontSize);
    return width;
}

int TextRenderer::getTextHeight(const std::string& text, int fontSize) {
    auto [width, height] = getTextSize(text, fontSize);
    return height;
}

void TextRenderer::renderMultilineText(SDL_Renderer* renderer, const std::vector<std::string>& lines,
                                     int x, int y, int fontSize, const GameColor* color,
                                     int lineSpacing) {
    if (lines.empty()) {
        return;
    }
    
    int actualFontSize = fontSize > 0 ? fontSize : defaultFontSize;
    int currentY = y;
    
    for (const std::string& line : lines) {
        if (!line.empty()) {  // Skip empty lines
            renderText(renderer, line, x, currentY, actualFontSize, color);
        }
        currentY += getTextHeight(line.empty() ? "A" : line, actualFontSize) + lineSpacing;
    }
}

void TextRenderer::clearCache() {
    fontCache.clear();
}

size_t TextRenderer::getFontCacheSize() const {
    return fontCache.size();
}

SDL_Texture* TextRenderer::createTextTexture(SDL_Renderer* renderer, const std::string& text,
                                            TTF_Font* font, const GameColor& color) {
    SDL_Color sdlColor = {color.r, color.g, color.b, color.a};
    
    SDL_Surface* surface = TTF_RenderText_Blended(font, text.c_str(), text.length(), sdlColor);
    if (!surface) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to render text surface: %s", SDL_GetError());
        return nullptr;
    }
    
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_DestroySurface(surface);
    
    if (!texture) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to create texture from surface: %s", SDL_GetError());
        return nullptr;
    }
    
    return texture;
}

} // namespace ui
} // namespace game 