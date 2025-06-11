#pragma once

#include "Event.hpp"

namespace game {
namespace events {

/**
 * Interface for components that want to receive events.
 */
class EventListener {
public:
    /**
     * Virtual destructor for proper inheritance
     */
    virtual ~EventListener() = default;

    /**
     * Called when an event is received.
     * @param event The event that was received
     */
    virtual void onEvent(const Event& event) = 0;
};

} // namespace events
} // namespace game 