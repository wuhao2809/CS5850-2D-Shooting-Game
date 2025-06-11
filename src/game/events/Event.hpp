#pragma once

#include <string>
#include <chrono>

namespace game {
namespace events {

/**
 * Base class for all events in the game engine.
 * Events are immutable to ensure thread safety and prevent modification after publishing.
 */
class Event {
public:
    /**
     * Initialize a new event.
     * @param type The type of event
     */
    explicit Event(const std::string& type);

    /**
     * Virtual destructor for proper inheritance
     */
    virtual ~Event() = default;

    /**
     * Get the event type.
     * @return The event type
     */
    const std::string& getType() const { return type_; }

    /**
     * Get the event timestamp.
     * @return The timestamp when the event was created (in milliseconds)
     */
    long getTimestamp() const { return timestamp_; }

private:
    const std::string type_;
    const long timestamp_;
};

} // namespace events
} // namespace game 