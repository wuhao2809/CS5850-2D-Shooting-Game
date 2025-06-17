/**
 * DestroyRequest component for request-based entity destruction.
 * 
 * C++ Implementation References:
 * - Python: /Lesson-41-CleanEvents/Python/src/game/ecs/components/destroy_request.py
 * - Java: /Lesson-41-CleanEvents/Java/src/game/ecs/components/DestroyRequest.java
 * 
 * Implementation of DestroyRequest component methods following the exact
 * same patterns established in the Python and Java reference implementations.
 */

#include "DestroyRequest.hpp"
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <SDL3/SDL.h>

namespace game {
namespace ecs {
namespace components {

// Constructors
DestroyRequest::DestroyRequest(Entity entity, const std::string& reason, float delay)
    : Component(entity), reason(reason.empty() ? "unknown" : reason), delay(delay), 
      processed(false), timestamp(std::chrono::steady_clock::now()) {
    SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, 
                "[DestroyRequest] Created component for entity %llu, reason='%s', delay=%.2fs", 
                entity.getId(), this->reason.c_str(), delay);
}

DestroyRequest::DestroyRequest(Entity entity, const std::string& reason)
    : DestroyRequest(entity, reason, 0.0f) {
    SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, 
                "[DestroyRequest] Created immediate destruction component for entity %llu, reason='%s'", 
                entity.getId(), this->reason.c_str());
}

DestroyRequest::DestroyRequest(Entity entity)
    : DestroyRequest(entity, "unknown", 0.0f) {
    SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, 
                "[DestroyRequest] Created component for entity %llu with default reason", 
                entity.getId());
}

// Methods
bool DestroyRequest::isReadyForDestruction() const {
    if (delay <= 0.0f) {
        return true;
    }
    
    float elapsed = getElapsedTime();
    return elapsed >= delay;
}

float DestroyRequest::getRemainingDelay() const {
    if (delay <= 0.0f) {
        return 0.0f;
    }
    
    float elapsed = getElapsedTime();
    float remaining = delay - elapsed;
    return std::max(0.0f, remaining);
}

float DestroyRequest::getElapsedTime() const {
    auto currentTime = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(currentTime - timestamp);
    return duration.count() / 1000000.0f; // Convert microseconds to seconds
}

void DestroyRequest::markProcessed() {
    processed = true;
    SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION,
                "[DestroyRequest] Marked as processed for entity %llu, reason='%s'", 
                getEntity().getId(), reason.c_str());
}

std::string DestroyRequest::toString() const {
    std::ostringstream oss;
    std::string status = processed ? "processed" : "pending";
    
    if (delay > 0.0f) {
        float remaining = getRemainingDelay();
        oss << "DestroyRequest(reason='" << reason << "', delay=" 
            << std::fixed << std::setprecision(2) << delay << "s, remaining=" 
            << remaining << "s, " << status << ")";
    } else {
        oss << "DestroyRequest(reason='" << reason << "', immediate, " << status << ")";
    }
    
    return oss.str();
}

} // namespace components
} // namespace ecs
} // namespace game 