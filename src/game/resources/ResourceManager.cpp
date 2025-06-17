#include "ResourceManager.hpp"
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <stdexcept>
#include <filesystem>

namespace fs = std::filesystem;

namespace game {
namespace resources {

// Initialize static members
std::unique_ptr<ResourceManager> ResourceManager::instance = nullptr;
std::mutex ResourceManager::mutex;

ResourceManager::ResourceManager() 
    : missingTexture(nullptr) {
}

ResourceManager& ResourceManager::getInstance() {
    std::lock_guard<std::mutex> lock(mutex);
    if (!instance) {
        instance = std::unique_ptr<ResourceManager>(new ResourceManager());
    }
    return *instance;
}

void ResourceManager::setAssetsDirectory(const std::string& directory) {
    std::lock_guard<std::mutex> lock(mutex);
    assetsDirectory = directory;
}

void ResourceManager::initMissingTexture(SDL_Renderer* renderer) {
    try {
        std::string missingTexturePath;
        if (!assetsDirectory.empty()) {
            missingTexturePath = (fs::path(assetsDirectory) / "images" / "grrr.png").lexically_normal().string();
        } else {
            missingTexturePath = (fs::path("GameAssets") / "images" / "grrr.png").lexically_normal().string();
        }
        
        if (fs::exists(missingTexturePath)) {
            missingTexture = std::make_shared<Image>(missingTexturePath, renderer);
        } else {
            missingTexture = createCheckerboardTexture(64, 64, renderer);
        }
    } catch (const std::exception&) {
        missingTexture = createCheckerboardTexture(64, 64, renderer);
    }
}

std::shared_ptr<Image> ResourceManager::createCheckerboardTexture(int width, int height, SDL_Renderer* renderer) {
    SDL_Surface* surface = SDL_CreateSurface(width, height, SDL_PIXELFORMAT_RGBA8888);
    if (!surface) {
        throw std::runtime_error("Failed to create surface for checkerboard texture");
    }
    
    const SDL_PixelFormatDetails* formatDetails = SDL_GetPixelFormatDetails(surface->format);
    if (!formatDetails) {
        SDL_DestroySurface(surface);
        throw std::runtime_error("Failed to get pixel format details");
    }
    
    const int boxSize = 8;
    SDL_Color magenta = {255, 0, 255, 255};
    SDL_Color black = {0, 0, 0, 255};
    
    for (int y = 0; y < height; y += boxSize * 2) {
        for (int x = 0; x < width; x += boxSize * 2) {
            for (int by = 0; by < boxSize && y + by < height; ++by) {
                for (int bx = 0; bx < boxSize && x + bx < width; ++bx) {
                    SDL_Rect rect = {x + bx, y + by, boxSize, boxSize};
                    SDL_Color color = ((x / boxSize + y / boxSize) % 2 == 0) ? magenta : black;
                    SDL_FillSurfaceRect(surface, &rect, SDL_MapRGBA(formatDetails, nullptr, color.r, color.g, color.b, color.a));
                }
            }
            if (x + boxSize < width && y + boxSize < height) {
                for (int by = 0; by < boxSize && y + boxSize + by < height; ++by) {
                    for (int bx = 0; bx < boxSize && x + boxSize + bx < width; ++bx) {
                        SDL_Rect rect = {x + boxSize + bx, y + boxSize + by, boxSize, boxSize};
                        SDL_Color color = ((x / boxSize + y / boxSize) % 2 == 0) ? magenta : black;
                        SDL_FillSurfaceRect(surface, &rect, SDL_MapRGBA(formatDetails, nullptr, color.r, color.g, color.b, color.a));
                    }
                }
            }
        }
    }
    
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_DestroySurface(surface);
    
    if (!texture) {
        throw std::runtime_error("Failed to create texture for checkerboard texture");
    }
    
    return std::make_shared<Image>("checkerboard_texture", renderer);
}

std::string ResourceManager::resolveImagePath(const std::string& imageName) const {
    if (!assetsDirectory.empty()) {
        return (fs::path(assetsDirectory) / "images" / imageName).lexically_normal().string();
    }
    return (fs::path("GameAssets") / "images" / imageName).lexically_normal().string();
}

std::shared_ptr<Image> ResourceManager::loadImage(const std::string& imageName, SDL_Renderer* renderer) {
    // Create a scoped block to ensure the mutex is released before calling getMissingTexture
    // This prevents deadlock since getMissingTexture also needs to acquire the same mutex
    {
        std::lock_guard<std::mutex> lock(mutex);
        auto it = imageCache.find(imageName);
        if (it != imageCache.end()) {
            return it->second;
        }
        
        try {
            std::string path = resolveImagePath(imageName);
            auto image = std::make_shared<Image>(path, renderer);
            imageCache[imageName] = image;
            return image;
        } catch (const std::exception&) {
            // Lock is automatically released when we exit this block
        }
    }  // Lock is released here
    
    return getMissingTexture(renderer);  // Now we can safely acquire the lock again
}

std::shared_ptr<Image> ResourceManager::getMissingTexture(SDL_Renderer* renderer) const {
    std::lock_guard<std::mutex> lock(mutex);
    if (!missingTexture) {
        const_cast<ResourceManager*>(this)->initMissingTexture(renderer);
    }
    return missingTexture;
}

void ResourceManager::clearCache() {
    std::lock_guard<std::mutex> lock(mutex);
    imageCache.clear();
}

} // namespace resources
} // namespace game 