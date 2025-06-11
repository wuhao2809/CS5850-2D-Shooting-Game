#include <gtest/gtest.h>
#include <string>
#include <unordered_map>
#include <memory>

// Mock HUD class for testing data management without SDL dependencies
class MockHUD {
public:
    struct Color {
        uint8_t r, g, b;
    };

    MockHUD(int screenWidth, int screenHeight) 
        : screenWidth(screenWidth), screenHeight(screenHeight), visible(true) {}
    
    void addText(const std::string& id, const std::string& text, int x, int y) {
        if (textElements.find(id) != textElements.end()) {
            throw std::invalid_argument("Text element with id '" + id + "' already exists");
        }
        textElements[id] = TextElement{text, x, y, 20, {255, 255, 255}};
    }
    
    void addText(const std::string& id, const std::string& text, int x, int y, int fontSize) {
        if (textElements.find(id) != textElements.end()) {
            throw std::invalid_argument("Text element with id '" + id + "' already exists");
        }
        textElements[id] = TextElement{text, x, y, fontSize, {255, 255, 255}};
    }
    
    void updateText(const std::string& id, const std::string& newText) {
        auto it = textElements.find(id);
        if (it == textElements.end()) {
            throw std::invalid_argument("Text element with id '" + id + "' not found");
        }
        it->second.text = newText;
    }
    
    void removeText(const std::string& id) {
        if (textElements.erase(id) == 0) {
            throw std::invalid_argument("Text element with id '" + id + "' not found");
        }
    }
    
    void setPosition(const std::string& id, int x, int y) {
        auto it = textElements.find(id);
        if (it == textElements.end()) {
            throw std::invalid_argument("Text element with id '" + id + "' not found");
        }
        it->second.x = x;
        it->second.y = y;
    }
    
    void setFontSize(const std::string& id, int size) {
        auto it = textElements.find(id);
        if (it == textElements.end()) {
            throw std::invalid_argument("Text element with id '" + id + "' not found");
        }
        it->second.fontSize = size;
    }
    
    void setColor(const std::string& id, const Color& color) {
        auto it = textElements.find(id);
        if (it == textElements.end()) {
            throw std::invalid_argument("Text element with id '" + id + "' not found");
        }
        it->second.color = color;
    }
    
    void toggleVisibility() {
        visible = !visible;
    }
    
    void setVisible(bool v) {
        visible = v;
    }
    
    bool isVisible() const {
        return visible;
    }
    
    void clear() {
        textElements.clear();
    }
    
    size_t getFontCacheSize() const {
        return 0; // Mock implementation
    }

private:
    struct TextElement {
        std::string text;
        int x;
        int y;
        int fontSize;
        Color color;
    };
    
    int screenWidth;
    int screenHeight;
    std::unordered_map<std::string, TextElement> textElements;
    bool visible;
};

class HUDTest : public ::testing::Test {
protected:
    void SetUp() override {
        hud = std::make_unique<MockHUD>(800, 600);
    }
    
    void TearDown() override {
        hud.reset();
    }
    
    std::unique_ptr<MockHUD> hud;
};

// Basic Functionality Tests
TEST_F(HUDTest, ConstructorInitializesCorrectly) {
    EXPECT_TRUE(hud->isVisible());
    EXPECT_EQ(hud->getFontCacheSize(), 0);
}

TEST_F(HUDTest, AddTextWithDefaultFontSize) {
    EXPECT_NO_THROW(hud->addText("test1", "Hello", 100, 100));
    EXPECT_THROW(hud->addText("test1", "Duplicate", 300, 300), std::invalid_argument);
}

TEST_F(HUDTest, AddTextWithCustomFontSize) {
    EXPECT_NO_THROW(hud->addText("test1", "Hello", 100, 100, 36));
    EXPECT_NO_THROW(hud->addText("test2", "World", 200, 200, 48));
}

// Text Management Tests
TEST_F(HUDTest, UpdateText) {
    hud->addText("test", "Original", 100, 100);
    EXPECT_NO_THROW(hud->updateText("test", "Updated"));
    EXPECT_THROW(hud->updateText("nonexistent", "Text"), std::invalid_argument);
}

TEST_F(HUDTest, RemoveText) {
    hud->addText("test", "Hello", 100, 100);
    EXPECT_NO_THROW(hud->removeText("test"));
    EXPECT_THROW(hud->removeText("test"), std::invalid_argument);
}

TEST_F(HUDTest, Clear) {
    hud->addText("test1", "Hello", 100, 100);
    hud->addText("test2", "World", 200, 200);
    hud->clear();
    EXPECT_THROW(hud->updateText("test1", "Text"), std::invalid_argument);
    EXPECT_THROW(hud->updateText("test2", "Text"), std::invalid_argument);
}

// Layout and Styling Tests
TEST_F(HUDTest, SetPosition) {
    hud->addText("test", "Hello", 100, 100);
    EXPECT_NO_THROW(hud->setPosition("test", 200, 200));
    EXPECT_THROW(hud->setPosition("nonexistent", 300, 300), std::invalid_argument);
}

TEST_F(HUDTest, SetFontSize) {
    hud->addText("test", "Hello", 100, 100);
    EXPECT_NO_THROW(hud->setFontSize("test", 36));
    EXPECT_THROW(hud->setFontSize("nonexistent", 48), std::invalid_argument);
}

TEST_F(HUDTest, SetColor) {
    hud->addText("test", "Hello", 100, 100);
    EXPECT_NO_THROW(hud->setColor("test", {255, 0, 0}));
    EXPECT_THROW(hud->setColor("nonexistent", {0, 0, 255}), std::invalid_argument);
}

// Visibility Tests
TEST_F(HUDTest, VisibilityControl) {
    EXPECT_TRUE(hud->isVisible());
    
    hud->toggleVisibility();
    EXPECT_FALSE(hud->isVisible());
    
    hud->toggleVisibility();
    EXPECT_TRUE(hud->isVisible());
    
    hud->setVisible(false);
    EXPECT_FALSE(hud->isVisible());
    
    hud->setVisible(true);
    EXPECT_TRUE(hud->isVisible());
}

// Multiple Elements Test
TEST_F(HUDTest, MultipleTextElements) {
    EXPECT_NO_THROW(hud->addText("test1", "First", 100, 100));
    EXPECT_NO_THROW(hud->addText("test2", "Second", 200, 200, 36));
    EXPECT_NO_THROW(hud->addText("test3", "Third", 300, 300, 48));
    
    EXPECT_NO_THROW(hud->updateText("test1", "Updated First"));
    EXPECT_NO_THROW(hud->setPosition("test2", 250, 250));
    EXPECT_NO_THROW(hud->setColor("test3", {255, 0, 0}));
    
    EXPECT_NO_THROW(hud->removeText("test1"));
    EXPECT_THROW(hud->updateText("test1", "Text"), std::invalid_argument);
}