#pragma once

#include <SDL3/SDL.h>
#include <string>
#include <memory>

namespace game {
namespace resources {

/**
 * Wrapper class for SDL3 texture handling.
 * Provides a consistent interface for image operations across the engine.
 */
class Image {
public:
    enum class ScalingMode {
        STRETCH,
        TILE,
        CENTER
    };

    /**
     * Construct an Image and attempt to load it from the given path
     * @throws std::runtime_error if image fails to load
     */
    Image(const std::string& path, SDL_Renderer* renderer);
    
    // Rule of five
    ~Image();
    Image(const Image&) = delete;
    Image& operator=(const Image&) = delete;
    Image(Image&&) noexcept = default;
    Image& operator=(Image&&) noexcept = default;
    
    /**
     * Load an image from the given path
     * @throws std::runtime_error if image fails to load
     */
    void load(const std::string& path, SDL_Renderer* renderer);
    
    /**
     * Render the image at the specified position and size
     */
    void render(SDL_Renderer* renderer, int x, int y, int width, int height);
    
    /**
     * Render the image with the specified scaling mode
     */
    void renderScaled(SDL_Renderer* renderer, int x, int y, int width, int height, ScalingMode mode);
    
    /**
     * Get the path of the loaded image
     */
    const std::string& getPath() const { return path; }
    
    /**
     * Check if the image is successfully loaded
     */
    bool isLoaded() const { return texture != nullptr; }
    
    /**
     * Get the width of the image in pixels
     */
    int getWidth() const;
    
    /**
     * Get the height of the image in pixels
     */
    int getHeight() const;
    
    /**
     * Get the underlying SDL texture
     */
    SDL_Texture* getTexture() const { return texture; }

private:
    SDL_Texture* texture;
    std::string path;
};

} // namespace resources
} // namespace game 