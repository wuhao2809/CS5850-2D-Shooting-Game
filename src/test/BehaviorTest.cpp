#include <gtest/gtest.h>
#include "../game/behaviors/Behavior.hpp"
#include "../game/behaviors/StaticBehavior.hpp"
#include "../game/behaviors/BounceBehavior.hpp"
#include "../game/behaviors/BehaviorManager.hpp"
#include "../game/GameObject.hpp"
#include "../game/Color.hpp"
#include "../game/GameWorld.hpp"

using namespace game;
using namespace game::behaviors;

class BehaviorTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create a test world
        GameWorld::getInstance()->setWorldDimensions(800, 600);
    }
    
    void TearDown() override {
        // Clean up
    }
};

// Test StaticBehavior
TEST_F(BehaviorTest, StaticBehaviorTest) {
    // Create a game object with static behavior
    GameObject obj(100, 100, 40, 40, Color(0, 255, 0), "static");
    
    // Initial position
    float initialX = obj.getX();
    float initialY = obj.getY();
    
    // Update with some delta time
    obj.update(1.0f);
    
    // Position should not change
    EXPECT_FLOAT_EQ(obj.getX(), initialX);
    EXPECT_FLOAT_EQ(obj.getY(), initialY);
    
    // Try to set velocity
    obj.setBehavior("static");
    obj.update(1.0f);
    
    // Position should still not change
    EXPECT_FLOAT_EQ(obj.getX(), initialX);
    EXPECT_FLOAT_EQ(obj.getY(), initialY);
}

// Test BounceBehavior
TEST_F(BehaviorTest, BounceBehaviorTest) {
    // Create a game object with bounce behavior
    GameObject obj(100, 100, 40, 40, Color(255, 0, 0), "bounce");
    
    // Set initial velocity
    obj.setBehavior("bounce");
    auto behavior = dynamic_cast<BounceBehavior*>(obj.getBehavior());
    ASSERT_NE(behavior, nullptr);
    behavior->setVelocity(100.0, 100.0);
    
    // Initial position
    float initialX = obj.getX();
    float initialY = obj.getY();
    
    // Update with small delta time
    obj.update(0.1f);
    
    // Position should change
    EXPECT_NE(obj.getX(), initialX);
    EXPECT_NE(obj.getY(), initialY);
    
    // Test boundary bouncing
    // Move to right boundary and set velocity towards boundary
    obj.setPosition(760, 100); // 800 - 40 = 760 (right boundary)
    obj.setBehavior("bounce");
    behavior = dynamic_cast<BounceBehavior*>(obj.getBehavior());
    ASSERT_NE(behavior, nullptr);
    behavior->setVelocity(100.0, 0.0);
    obj.update(0.1f);
    
    // Should bounce back
    EXPECT_LT(obj.getX(), 760);
    
    // Move to bottom boundary and set velocity towards boundary
    obj.setPosition(100, 560); // 600 - 40 = 560 (bottom boundary)
    obj.setBehavior("bounce");
    behavior = dynamic_cast<BounceBehavior*>(obj.getBehavior());
    ASSERT_NE(behavior, nullptr);
    behavior->setVelocity(0.0, 100.0);
    obj.update(0.1f);
    
    // Should bounce back
    EXPECT_LT(obj.getY(), 560);
}

// Test BehaviorManager
TEST_F(BehaviorTest, BehaviorManagerTest) {
    auto& manager = BehaviorManager::getInstance();
    
    // Test available behaviors
    auto behaviors = manager.getAvailableBehaviors();
    EXPECT_FALSE(behaviors.empty());
    EXPECT_TRUE(std::find(behaviors.begin(), behaviors.end(), "static") != behaviors.end());
    EXPECT_TRUE(std::find(behaviors.begin(), behaviors.end(), "bounce") != behaviors.end());
    
    // Test unknown behavior
    GameObject obj(100, 100, 40, 40, Color(0, 0, 255), "unknown");
    EXPECT_EQ(obj.getBehaviorName(), "unknown");
    
    // Test case insensitivity
    obj.setBehavior("STATIC");
    EXPECT_EQ(obj.getBehaviorName(), "STATIC");
    
    obj.setBehavior("BOUNCE");
    EXPECT_EQ(obj.getBehaviorName(), "BOUNCE");
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
} 