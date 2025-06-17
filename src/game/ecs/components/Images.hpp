#pragma once

#include "../Component.hpp"
#include <string>
#include <vector>
#include <SDL3/SDL.h>

namespace game {
namespace ecs {
namespace components {

class Images : public Component {
public:
    Images(const Entity& entity) : Component(entity) {}
    Images(const Entity& entity, const std::vector<std::string>& imageNames) 
        : Component(entity), imageNames_(imageNames) {}

    // Add an image to the list
    void addImage(const std::string& imageName) {
        imageNames_.push_back(imageName);
    }

    // Get the current image name
    const std::string& getCurrentImageName() const {
        if (imageNames_.empty()) {
            static const std::string empty;
            return empty;
        }
        return imageNames_[currentIndex_];
    }

    // Switch to the next image
    void nextImage() {
        if (!imageNames_.empty()) {
            currentIndex_ = (currentIndex_ + 1) % imageNames_.size();
        }
    }

    // Switch to the previous image
    void previousImage() {
        if (!imageNames_.empty()) {
            currentIndex_ = (currentIndex_ - 1 + imageNames_.size()) % imageNames_.size();
        }
    }

    // Set the current image by index
    void setCurrentImage(size_t index) {
        if (!imageNames_.empty()) {
            currentIndex_ = index % imageNames_.size();
        }
    }

    // Get the number of images
    size_t getImageCount() const {
        return imageNames_.size();
    }

    // Get all image names
    const std::vector<std::string>& getImageNames() const {
        return imageNames_;
    }

private:
    std::vector<std::string> imageNames_;
    size_t currentIndex_ = 0;
};

} // namespace components
} // namespace ecs
} // namespace game 