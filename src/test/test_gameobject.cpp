#include <gtest/gtest.h>
#include <SDL3/SDL.h>
#include "../game/GameObject.hpp"
#include "../game/Color.hpp"

namespace game {
namespace test {

class GameObjectTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Initialize SDL
        SDL_Init(SDL_INIT_VIDEO);
        
        // Create a window and renderer for testing
        window = SDL_CreateWindow("Test", 800, 600, 0);
        renderer = SDL_CreateRenderer(window, "test");
        
        // Create test objects
        testColor = Color(255, 0, 0); // Red color
        gameObject = new GameObject(10.0f, 20.0f, 30.0f, 40.0f, testColor);
    }
    
    void TearDown() override {
        delete gameObject;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }
    
    SDL_Window* window;
    SDL_Renderer* renderer;
    Color testColor;
    GameObject* gameObject;
};

TEST_F(GameObjectTest, ConstructorInitializesProperties) {
    EXPECT_FLOAT_EQ(gameObject->getX(), 10.0f);
    EXPECT_FLOAT_EQ(gameObject->getY(), 20.0f);
    EXPECT_FLOAT_EQ(gameObject->getWidth(), 30.0f);
    EXPECT_FLOAT_EQ(gameObject->getHeight(), 40.0f);
    EXPECT_EQ(gameObject->getColor().r, testColor.r);
    EXPECT_EQ(gameObject->getColor().g, testColor.g);
    EXPECT_EQ(gameObject->getColor().b, testColor.b);
    EXPECT_TRUE(gameObject->isActive());
}

TEST_F(GameObjectTest, SetPositionUpdatesCoordinates) {
    gameObject->setPosition(100.0f, 200.0f);
    EXPECT_FLOAT_EQ(gameObject->getX(), 100.0f);
    EXPECT_FLOAT_EQ(gameObject->getY(), 200.0f);
}

TEST_F(GameObjectTest, SetSizeUpdatesDimensions) {
    gameObject->setSize(50.0f, 60.0f);
    EXPECT_FLOAT_EQ(gameObject->getWidth(), 50.0f);
    EXPECT_FLOAT_EQ(gameObject->getHeight(), 60.0f);
}

TEST_F(GameObjectTest, SetColorUpdatesColor) {
    Color newColor(0, 255, 0); // Green color
    gameObject->setColor(newColor);
    EXPECT_EQ(gameObject->getColor().r, newColor.r);
    EXPECT_EQ(gameObject->getColor().g, newColor.g);
    EXPECT_EQ(gameObject->getColor().b, newColor.b);
}

TEST_F(GameObjectTest, SetActiveTogglesActiveState) {
    gameObject->setActive(false);
    EXPECT_FALSE(gameObject->isActive());
    
    gameObject->setActive(true);
    EXPECT_TRUE(gameObject->isActive());
}

TEST_F(GameObjectTest, RenderDoesNotThrow) {
    EXPECT_NO_THROW(gameObject->render(renderer));
}

TEST_F(GameObjectTest, UpdateMethodDoesNotThrow) {
    EXPECT_NO_THROW(gameObject->update(0.016f));
}

} // namespace test
} // namespace game

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
} 