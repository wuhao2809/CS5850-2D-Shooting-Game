#pragma once

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <memory>
#include <string>
#include <unordered_map>
#include <mutex>
#include "Image.hpp"

namespace game {
namespace resources {

/**
 * Manages image resources using a singleton pattern
 * Handles loading and caching of images
 */
class ResourceManager {
public:
    static ResourceManager& getInstance();
    
    void setAssetsDirectory(const std::string& directory);
    void initMissingTexture(SDL_Renderer* renderer);
    std::shared_ptr<Image> loadImage(const std::string& imageName, SDL_Renderer* renderer);
    std::shared_ptr<Image> getMissingTexture(SDL_Renderer* renderer) const;
    void clearCache();
    
    // Make destructor public but prevent direct deletion
    ~ResourceManager() = default;
    
private:
    ResourceManager();
    
    // Prevent copying
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;
    
    std::string resolveImagePath(const std::string& imageName) const;
    std::shared_ptr<Image> createCheckerboardTexture(int width, int height, SDL_Renderer* renderer);
    
    static std::unique_ptr<ResourceManager> instance;
    static std::mutex mutex;
    
    std::string assetsDirectory;
    std::unordered_map<std::string, std::shared_ptr<Image>> imageCache;
    mutable std::shared_ptr<Image> missingTexture;  // Made mutable for const methods
};

} // namespace resources
} // namespace game 