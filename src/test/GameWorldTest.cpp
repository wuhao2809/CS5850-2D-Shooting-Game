#include <gtest/gtest.h>
#include <fstream>
#include <filesystem>
#include <nlohmann/json.hpp>
#include "../game/GameWorld.hpp"
#include "../game/GameObject.hpp"
#include "../game/Color.hpp"
#include "../game/behaviors/BehaviorManager.hpp"

using namespace game;

class GameWorldTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create test data
        nlohmann::json testData = {
            {"world", {
                {"width", 800},
                {"height", 600}
            }},
            {"objects", {
                {
                    {"name", "test_object"},
                    {"size", {
                        {"width", 40},
                        {"height", 40}
                    }},
                    {"position", {
                        {"x", 100},
                        {"y", 100}
                    }},
                    {"velocity", {
                        {"x", 1},
                        {"y", 1}
                    }},
                    {"color", {
                        {"r", 0},
                        {"g", 0},
                        {"b", 255}
                    }},
                    {"behavior", "bounce"}
                }
            }}
        };
        
        // Create a test-specific directory
        testDir = std::filesystem::temp_directory_path() / "GameWorld";
        testJsonPath = testDir / "GameData.json";
        
        std::cout << "Creating test directory: " << testDir << std::endl;
        std::cout << "Creating test file: " << testJsonPath << std::endl;
        
        // Create test directory if it doesn't exist
        std::filesystem::create_directories(testDir);
        
        // Write test data to file
        std::ofstream file(testJsonPath);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to create test file: " + testJsonPath.string());
        }
        file << testData.dump(4);
        file.close();
        
        // Verify file was created
        if (std::filesystem::exists(testJsonPath)) {
            std::cout << "Test file created successfully" << std::endl;
            std::ifstream verifyFile(testJsonPath);
            std::string contents((std::istreambuf_iterator<char>(verifyFile)),
                                std::istreambuf_iterator<char>());
            std::cout << "File contents: " << contents << std::endl;
        } else {
            std::cerr << "Failed to create test file" << std::endl;
        }

        // Reset GameWorld for testing
        GameWorld::resetForTesting();
        // Set assets directory before initialization
        GameWorld::setAssetsDirectory(testDir.string());
    }
    
    void TearDown() override {
        // Clean up test files
        std::filesystem::remove(testJsonPath);
        std::filesystem::remove(testDir);
        
        // Reset singleton
        GameWorld::resetForTesting();
    }
    
    std::filesystem::path testDir;
    std::filesystem::path testJsonPath;
};

TEST_F(GameWorldTest, Initialization) {
    // Initialize world
    GameWorld::initialize();
    auto world = GameWorld::getInstance();
    
    // Test world dimensions
    EXPECT_EQ(world->getWorldWidth(), 800);
    EXPECT_EQ(world->getWorldHeight(), 600);
    
    // Test object creation
    auto objects = world->getObjects();
    EXPECT_EQ(objects.size(), 1);
    
    // Test object properties
    auto obj = world->getObjectByName("test_object");
    ASSERT_NE(obj, nullptr);
    EXPECT_EQ(obj->getWidth(), 40);
    EXPECT_EQ(obj->getHeight(), 40);
    EXPECT_EQ(obj->getX(), 100);
    EXPECT_EQ(obj->getY(), 100);
    EXPECT_EQ(obj->getBehaviorName(), "bounce");
    const auto& color = obj->getColor();
    EXPECT_EQ(color.r, 0);
    EXPECT_EQ(color.g, 0);
    EXPECT_EQ(color.b, 255);
}

TEST_F(GameWorldTest, SingletonPattern) {
    // Get multiple instances
    auto world1 = GameWorld::getInstance();
    auto world2 = GameWorld::getInstance();
    
    // Test that both pointers point to the same instance
    EXPECT_EQ(world1, world2);
}

TEST_F(GameWorldTest, ObjectManagement) {
    // Initialize world
    GameWorld::initialize();
    auto world = GameWorld::getInstance();
    
    // Test initial state
    auto initialObjects = world->getObjects();
    size_t initialCount = initialObjects.size();
    
    // Add new object
    auto newObj = std::make_shared<GameObject>(200, 200, 50, 50, Color(255, 0, 0), "static");
    newObj->setName("new_object");
    world->addObject(newObj);
    
    // Verify object was added
    auto objects = world->getObjects();
    EXPECT_EQ(objects.size(), initialCount + 1);
    auto foundObj = world->getObjectByName("new_object");
    EXPECT_NE(foundObj, nullptr);
    
    // Remove object
    world->removeObject(newObj);
    objects = world->getObjects();
    EXPECT_EQ(objects.size(), initialCount);
    foundObj = world->getObjectByName("new_object");
    EXPECT_EQ(foundObj, nullptr);
}

TEST_F(GameWorldTest, ObjectUpdate) {
    // Initialize world
    GameWorld::initialize();
    auto world = GameWorld::getInstance();
    
    auto obj = world->getObjectByName("test_object");
    ASSERT_NE(obj, nullptr);
    
    // Store initial position
    float initialX = obj->getX();
    float initialY = obj->getY();
    
    // Update with delta time
    world->update(1.0f);  // 1 second
    
    // Check position has changed based on velocity
    EXPECT_NE(obj->getX(), initialX);  // Position should change with bounce behavior
    EXPECT_NE(obj->getY(), initialY);
}

TEST_F(GameWorldTest, Clear) {
    // Initialize world
    GameWorld::initialize();
    auto world = GameWorld::getInstance();
    
    // Add another object
    auto newObj = std::make_shared<GameObject>(100, 100, 20, 20, Color(0, 255, 0), "static");
    newObj->setName("another_object");
    world->addObject(newObj);
    
    // Verify objects exist
    EXPECT_EQ(world->getObjects().size(), 2);
    
    // Clear world
    world->clear();
    
    // Verify all objects removed
    EXPECT_EQ(world->getObjects().size(), 0);
    EXPECT_EQ(world->getObjectByName("test_object"), nullptr);
    EXPECT_EQ(world->getObjectByName("another_object"), nullptr);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
} 