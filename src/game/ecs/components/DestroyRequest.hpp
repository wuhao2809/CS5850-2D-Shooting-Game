/**
 * DestroyRequest component for request-based entity destruction.
 * 
 * C++ Implementation References:
 * - Python: /Lesson-41-CleanEvents/Python/src/game/ecs/components/destroy_request.py
 * - Java: /Lesson-41-CleanEvents/Java/src/game/ecs/components/DestroyRequest.java
 * 
 * This component provides controlled entity destruction with:
 * - Reason tracking for debugging/analytics
 * - Delay support for timed destruction
 * - Processing state management
 * 
 * Part of the hybrid event/ECS architecture enhancement for Phase 3.
 * Enables request-based entity lifecycle management instead of direct destruction.
 * 
 * Usage:
 *     // Mark entity for immediate destruction
 *     componentManager.addComponent<DestroyRequest>(entity, "collision");
 *     
 *     // Mark entity for delayed destruction
 *     componentManager.addComponent<DestroyRequest>(entity, "expired", 1.0f);
 */

#pragma once

#include <chrono>
#include <string>
#include "../Entity.hpp"
#include "../Component.hpp"

namespace game {
namespace ecs {
namespace components {

class DestroyRequest : public Component {
private:
    std::string reason;                                     // Reason for destruction (for debugging/analytics)
    float delay;                                            // Delay in seconds before destruction (0.0 = immediate)
    std::chrono::steady_clock::time_point timestamp;       // Request creation time
    bool processed;                                         // Processing state

public:
    /**
     * Initialize a DestroyRequest component with reason and delay.
     * 
     * @param entity The entity this component belongs to
     * @param reason Reason for destruction (for debugging/analytics)
     * @param delay Delay in seconds before destruction (0.0 = immediate)
     */
    DestroyRequest(Entity entity, const std::string& reason, float delay);
    
    /**
     * Initialize a DestroyRequest component with reason (immediate destruction).
     * 
     * @param entity The entity this component belongs to
     * @param reason Reason for destruction (for debugging/analytics)
     */
    DestroyRequest(Entity entity, const std::string& reason);
    
    /**
     * Initialize a DestroyRequest component with default reason (immediate destruction).
     * 
     * @param entity The entity this component belongs to
     */
    explicit DestroyRequest(Entity entity);
    
    /**
     * Default destructor.
     */
    ~DestroyRequest() = default;
    
    // Copy and move constructors/operators
    DestroyRequest(const DestroyRequest&) = default;
    DestroyRequest& operator=(const DestroyRequest&) = default;
    DestroyRequest(DestroyRequest&&) = default;
    DestroyRequest& operator=(DestroyRequest&&) = default;
    
    /**
     * Check if the destruction request is ready to be processed.
     * 
     * @return True if enough time has passed for delayed destruction, false otherwise
     */
    bool isReadyForDestruction() const;
    
    /**
     * Get the remaining delay time before destruction.
     * 
     * @return Remaining delay in seconds (0.0 if ready for destruction)
     */
    float getRemainingDelay() const;
    
    /**
     * Get the elapsed time since this request was created.
     * 
     * @return Elapsed time in seconds
     */
    float getElapsedTime() const;
    
    /**
     * Mark this request as processed.
     */
    void markProcessed();
    
    // Getters
    const std::string& getReason() const { return reason; }
    float getDelay() const { return delay; }
    std::chrono::steady_clock::time_point getTimestamp() const { return timestamp; }
    bool isProcessed() const { return processed; }
    
    /**
     * Get a string representation of this component.
     */
    std::string toString() const;
};

} // namespace components
} // namespace ecs
} // namespace game 