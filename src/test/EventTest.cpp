#include <gtest/gtest.h>
#include "../game/events/Event.hpp"
#include "../game/events/KeyboardEvent.hpp"
#include "../game/events/EventListener.hpp"
#include "../game/events/EventManager.hpp"
#include <memory>
#include <string>
#include <thread>
#include <chrono>

using namespace game::events;

// Test fixture for Event tests
class EventTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Clear the event manager before each test
        EventManager::getInstance().clear();
    }
};

// Test Event class
TEST_F(EventTest, EventCreation) {
    Event event("test_event");
    EXPECT_EQ(event.getType(), "test_event");
    EXPECT_GT(event.getTimestamp(), 0);
}

// Test KeyboardEvent class
TEST_F(EventTest, KeyboardEventCreation) {
    KeyboardEvent event("A", "A", true);
    EXPECT_EQ(event.getType(), "keyboard");
    EXPECT_EQ(event.getKey(), "A");
    EXPECT_EQ(event.getKeyText(), "A");
    EXPECT_TRUE(event.isPressed());
}

// Test EventListener interface
class TestEventListener : public EventListener {
public:
    void onEvent(const Event& event) override {
        lastEvent = std::make_shared<Event>(event);
        eventCount++;
    }

    std::shared_ptr<Event> lastEvent;
    int eventCount = 0;
};

// Test EventManager subscription and publishing
TEST_F(EventTest, EventManagerSubscription) {
    auto& manager = EventManager::getInstance();
    auto listener = std::make_unique<TestEventListener>();

    // Subscribe to keyboard events
    manager.subscribe("keyboard", listener.get());
    EXPECT_EQ(manager.getListenerCount("keyboard"), 1);

    // Publish a keyboard event
    auto event = std::make_shared<KeyboardEvent>("A", "A", true);
    manager.publish(event);

    // Process the event
    manager.update();

    // Verify the listener received the event
    EXPECT_EQ(listener->eventCount, 1);
    EXPECT_EQ(listener->lastEvent->getType(), "keyboard");
}

// Test EventManager unsubscription
TEST_F(EventTest, EventManagerUnsubscription) {
    auto& manager = EventManager::getInstance();
    auto listener = std::make_unique<TestEventListener>();

    // Subscribe and then unsubscribe
    manager.subscribe("keyboard", listener.get());
    manager.unsubscribe("keyboard", listener.get());
    EXPECT_EQ(manager.getListenerCount("keyboard"), 0);

    // Publish an event
    auto event = std::make_shared<KeyboardEvent>("A", "A", true);
    manager.publish(event);
    manager.update();

    // Verify the listener did not receive the event
    EXPECT_EQ(listener->eventCount, 0);
}

// Test EventManager queue
TEST_F(EventTest, EventManagerQueue) {
    auto& manager = EventManager::getInstance();
    auto listener = std::make_unique<TestEventListener>();

    manager.subscribe("keyboard", listener.get());

    // Publish multiple events
    for (int i = 0; i < 5; i++) {
        auto event = std::make_shared<KeyboardEvent>("A", "A", true);
        manager.publish(event);
    }

    EXPECT_EQ(manager.getQueueSize(), 5);
    manager.update();
    EXPECT_EQ(manager.getQueueSize(), 0);
    EXPECT_EQ(listener->eventCount, 5);
}

// Test thread safety
TEST_F(EventTest, ThreadSafety) {
    auto& manager = EventManager::getInstance();
    auto listener = std::make_unique<TestEventListener>();
    manager.subscribe("keyboard", listener.get());

    // Create multiple threads that publish events
    std::vector<std::thread> threads;
    for (int i = 0; i < 10; i++) {
        threads.emplace_back([&manager, i]() {
            auto event = std::make_shared<KeyboardEvent>("A", "A", true);
            manager.publish(event);
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        });
    }

    // Wait for all threads to complete
    for (auto& thread : threads) {
        thread.join();
    }

    // Process all events
    manager.update();

    // Verify all events were processed
    EXPECT_EQ(listener->eventCount, 10);
}

#ifndef RUNNING_ALL_TESTS
int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
#endif 