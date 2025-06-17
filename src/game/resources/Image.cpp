#include "Image.hpp"
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <stdexcept>
#include <iostream>
#include <algorithm>

namespace game {
namespace resources {

Image::Image(const std::string& path, SDL_Renderer* renderer) 
    : texture(nullptr), path(path) {
    load(path, renderer);
}

Image::~Image() {
    if (texture) {
        SDL_DestroyTexture(texture);
        texture = nullptr;
    }
}

void Image::load(const std::string& path, SDL_Renderer* renderer) {
    this->path = path;
    
    if (!renderer) {
        std::cerr << "Failed to load image: " << path << " - Invalid renderer" << std::endl;
        throw std::runtime_error("Failed to load image: Invalid renderer");
    }
    
    // Load the image directly into a texture
    texture = IMG_LoadTexture(renderer, path.c_str());
    
    if (!texture) {
        std::cerr << "Failed to load image: " << path << " - " << SDL_GetError() << std::endl;
        throw std::runtime_error("Failed to load image: " + path);
    }
}

void Image::render(SDL_Renderer* renderer, int x, int y, int width, int height) {
    if (!texture || !renderer) return;
    
    SDL_FRect destRect = {
        static_cast<float>(x),
        static_cast<float>(y),
        static_cast<float>(width),
        static_cast<float>(height)
    };
    
    SDL_RenderTexture(renderer, texture, nullptr, &destRect);
}

void Image::renderScaled(SDL_Renderer* renderer, int x, int y, int width, int height, ScalingMode mode) {
    if (!texture || !renderer) return;
    
    float texWidth, texHeight;
    SDL_GetTextureSize(texture, &texWidth, &texHeight);
    
    switch (mode) {
        case ScalingMode::STRETCH:
            render(renderer, x, y, width, height);
            break;
            
        case ScalingMode::TILE: {
            // Calculate how many tiles we need
            int tilesX = static_cast<int>((width + texWidth - 1) / texWidth);
            int tilesY = static_cast<int>((height + texHeight - 1) / texHeight);
            
            // Draw each tile
            for (int ty = 0; ty < tilesY; ++ty) {
                for (int tx = 0; tx < tilesX; ++tx) {
                    int posX = x + static_cast<int>(tx * texWidth);
                    int posY = y + static_cast<int>(ty * texHeight);
                    int tileW = std::min(static_cast<int>(texWidth), width - static_cast<int>(tx * texWidth));
                    int tileH = std::min(static_cast<int>(texHeight), height - static_cast<int>(ty * texHeight));
                    
                    render(renderer, posX, posY, tileW, tileH);
                }
            }
            break;
        }
            
        case ScalingMode::CENTER: {
            // Center the image without scaling
            int centerX = x + static_cast<int>((width - texWidth) / 2);
            int centerY = y + static_cast<int>((height - texHeight) / 2);
            render(renderer, centerX, centerY, static_cast<int>(texWidth), static_cast<int>(texHeight));
            break;
        }
    }
}

int Image::getWidth() const {
    if (!texture) return 0;
    float width;
    SDL_GetTextureSize(texture, &width, nullptr);
    return static_cast<int>(width);
}

int Image::getHeight() const {
    if (!texture) return 0;
    float height;
    SDL_GetTextureSize(texture, nullptr, &height);
    return static_cast<int>(height);
}

} // namespace resources
} // namespace game 