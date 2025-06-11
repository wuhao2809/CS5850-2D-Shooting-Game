#pragma once

#include "Event.hpp"
#include "EventListener.hpp"
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <mutex>

namespace game {
namespace events {

/**
 * Manages event subscriptions and publishing.
 * Thread-safe implementation using mutexes and thread-safe collections.
 */
class EventManager {
public:
    /**
     * Get the singleton instance of EventManager.
     * @return The EventManager instance
     */
    static EventManager& getInstance();

    /**
     * Subscribe a listener to a specific event type.
     * @param eventType The type of event to listen for
     * @param listener The listener to notify when events occur
     */
    void subscribe(const std::string& eventType, EventListener* listener);

    /**
     * Unsubscribe a listener from a specific event type.
     * @param eventType The type of event to stop listening for
     * @param listener The listener to remove
     */
    void unsubscribe(const std::string& eventType, EventListener* listener);

    /**
     * Publish an event to all subscribed listeners.
     * The event is added to a queue for processing on the next update.
     * @param event The event to publish
     */
    void publish(std::shared_ptr<Event> event);

    /**
     * Update the event system, processing all queued events.
     * This should be called once per frame from the game loop.
     */
    void update();

    /**
     * Clear all listeners and queued events.
     * Useful for cleanup or resetting the system.
     */
    void clear();

    /**
     * Get the number of listeners for a specific event type.
     * @param eventType The type of event
     * @return The number of listeners
     */
    size_t getListenerCount(const std::string& eventType) const;

    /**
     * Get the number of events currently in the queue.
     * @return The number of queued events
     */
    size_t getQueueSize() const;

private:
    // Private constructor for singleton pattern
    EventManager() = default;
    // Delete copy constructor and assignment operator
    EventManager(const EventManager&) = delete;
    EventManager& operator=(const EventManager&) = delete;

    std::unordered_map<std::string, std::unordered_set<EventListener*>> listeners_;
    std::queue<std::shared_ptr<Event>> eventQueue_;
    mutable std::mutex listenersMutex_;
    mutable std::mutex queueMutex_;
};

} // namespace events
} // namespace game 