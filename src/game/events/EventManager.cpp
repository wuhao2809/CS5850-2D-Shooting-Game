#include "EventManager.hpp"
#include <iostream>

namespace game {
namespace events {

EventManager& EventManager::getInstance() {
    static EventManager instance;
    return instance;
}

void EventManager::subscribe(const std::string& eventType, EventListener* listener) {
    std::lock_guard<std::mutex> lock(listenersMutex_);
    listeners_[eventType].insert(listener);
}

void EventManager::unsubscribe(const std::string& eventType, EventListener* listener) {
    std::lock_guard<std::mutex> lock(listenersMutex_);
    auto it = listeners_.find(eventType);
    if (it != listeners_.end()) {
        it->second.erase(listener);
        if (it->second.empty()) {
            listeners_.erase(it);
        }
    }
}

void EventManager::publish(std::shared_ptr<Event> event) {
    std::lock_guard<std::mutex> lock(queueMutex_);
    eventQueue_.push(event);
}

void EventManager::update() {
    while (true) {
        // Get next event from queue (thread-safe)
        std::shared_ptr<Event> event;
        {
            std::lock_guard<std::mutex> lock(queueMutex_);
            if (eventQueue_.empty()) {
                break;
            }
            event = eventQueue_.front();
            eventQueue_.pop();
        }

        // Get listeners for this event type (thread-safe)
        std::unordered_set<EventListener*> listeners;
        {
            std::lock_guard<std::mutex> lock(listenersMutex_);
            auto it = listeners_.find(event->getType());
            if (it != listeners_.end()) {
                listeners = it->second;
            }
        }

        // Notify all listeners
        for (auto listener : listeners) {
            try {
                listener->onEvent(*event);
            } catch (const std::exception& e) {
                std::cerr << "Error processing event: " << e.what() << std::endl;
            }
        }
    }
}

void EventManager::clear() {
    std::lock_guard<std::mutex> lock1(listenersMutex_);
    std::lock_guard<std::mutex> lock2(queueMutex_);
    listeners_.clear();
    while (!eventQueue_.empty()) {
        eventQueue_.pop();
    }
}

size_t EventManager::getListenerCount(const std::string& eventType) const {
    std::lock_guard<std::mutex> lock(listenersMutex_);
    auto it = listeners_.find(eventType);
    return it != listeners_.end() ? it->second.size() : 0;
}

size_t EventManager::getQueueSize() const {
    std::lock_guard<std::mutex> lock(queueMutex_);
    return eventQueue_.size();
}

} // namespace events
} // namespace game 