#include <gtest/gtest.h>
#include "../game/Timer.hpp"
#include <thread>
#include <chrono>
#include <iostream>
#include <iomanip>

class TimerTest : public ::testing::Test {
protected:
    Timer* timer;

    void SetUp() override {
        timer = new Timer(60);  // 60 FPS target
    }

    void TearDown() override {
        delete timer;
    }
};

// Test basic frame timing
TEST_F(TimerTest, BasicFrameTiming) {
    timer->startFrame();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    timer->waitForFrameEnd();
    
    // Frame time should be close to target (16.67ms for 60 FPS)
    double frameTime = timer->getElapsedTime();
    EXPECT_NEAR(frameTime, 1.0/60.0, 0.015);  // Allow 15ms tolerance
}

// Test FPS calculation
TEST_F(TimerTest, FpsCalculation) {
    // Run several frames
    for (int i = 0; i < 10; i++) {
        timer->startFrame();
        std::this_thread::sleep_for(std::chrono::milliseconds(16));  // Simulate ~60 FPS
        timer->waitForFrameEnd();
    }
    
    // FPS should be close to 60
    int fps = timer->getFps();
    EXPECT_NEAR(fps, 60, 5);  // Allow 5 FPS tolerance
}

// Test frame time history
TEST_F(TimerTest, FrameTimeHistory) {
    // Run several frames with different timings
    for (int i = 0; i < 70; i++) {  // More than MAX_FRAME_HISTORY
        timer->startFrame();
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
        timer->waitForFrameEnd();
    }
    
    // Average frame time should be close to target
    double avgFrameTime = timer->getAverageFrameTime();
    EXPECT_NEAR(avgFrameTime, 1.0/60.0, 0.015);  // Allow 15ms tolerance
}

// Test target FPS change
TEST_F(TimerTest, TargetFpsChange) {
    // Change target FPS
    timer->setTargetFps(30);
    
    // Run a frame
    timer->startFrame();
    std::this_thread::sleep_for(std::chrono::milliseconds(30));
    timer->waitForFrameEnd();
    
    // Frame time should be close to new target (33.33ms for 30 FPS)
    double frameTime = timer->getElapsedTime();
    EXPECT_NEAR(frameTime, 1.0/30.0, 0.015);  // Allow 15ms tolerance
}

// Extended timing test at 30Hz
TEST(TimerExtendedTest, ThirtyHzTest) {
    Timer timer(30);
    int totalFrames = 0;
    auto startTime = std::chrono::steady_clock::now();
    auto lastPrint = startTime;
    const int durationSeconds = 5;

    std::cout << "\nRunning timer test at 30Hz for " << durationSeconds << " seconds..." << std::endl;
    
    while (true) {
        timer.startFrame();
        
        auto currentTime = std::chrono::steady_clock::now();
        auto elapsedTotal = std::chrono::duration<double>(currentTime - startTime).count();
        auto elapsedSinceLastPrint = std::chrono::duration<double>(currentTime - lastPrint).count();
        
        if (elapsedSinceLastPrint >= 1.0) {
            std::cout << std::fixed << std::setprecision(3)
                     << "Time: " << elapsedTotal << "s, "
                     << "FPS: " << timer.getFps() << ", "
                     << "Frame Time: " << timer.getAverageFrameTime() * 1000.0 << "ms"
                     << std::endl;
            lastPrint = currentTime;
        }
        
        if (elapsedTotal >= durationSeconds) {
            break;
        }
        
        totalFrames++;
        timer.waitForFrameEnd();
    }
    
    auto endTime = std::chrono::steady_clock::now();
    double totalTime = std::chrono::duration<double>(endTime - startTime).count();
    double averageFps = totalFrames / totalTime;
    
    std::cout << "\nTest Summary for 30Hz:" << std::endl;
    std::cout << "Total frames: " << totalFrames << std::endl;
    std::cout << "Total time: " << std::fixed << std::setprecision(3) << totalTime << "s" << std::endl;
    std::cout << "Average FPS: " << std::fixed << std::setprecision(1) << averageFps << std::endl;
    std::cout << "Average frame time: " << std::fixed << std::setprecision(3) 
              << timer.getAverageFrameTime() * 1000.0 << "ms" << std::endl;

    // Add some actual test assertions
    EXPECT_NEAR(totalTime, 5.0, 0.1);  // Test should run for ~5 seconds
    EXPECT_NEAR(averageFps, 30.0, 1.0);  // Average FPS should be close to 30
    EXPECT_NEAR(timer.getAverageFrameTime() * 1000.0, 33.33, 2.0);  // Frame time should be close to 33.33ms
}

// Extended timing test at 60Hz
TEST(TimerExtendedTest, SixtyHzTest) {
    Timer timer(60);
    int totalFrames = 0;
    auto startTime = std::chrono::steady_clock::now();
    auto lastPrint = startTime;
    const int durationSeconds = 5;

    std::cout << "\nRunning timer test at 60Hz for " << durationSeconds << " seconds..." << std::endl;
    
    while (true) {
        timer.startFrame();
        
        auto currentTime = std::chrono::steady_clock::now();
        auto elapsedTotal = std::chrono::duration<double>(currentTime - startTime).count();
        auto elapsedSinceLastPrint = std::chrono::duration<double>(currentTime - lastPrint).count();
        
        if (elapsedSinceLastPrint >= 1.0) {
            std::cout << std::fixed << std::setprecision(3)
                     << "Time: " << elapsedTotal << "s, "
                     << "FPS: " << timer.getFps() << ", "
                     << "Frame Time: " << timer.getAverageFrameTime() * 1000.0 << "ms"
                     << std::endl;
            lastPrint = currentTime;
        }
        
        if (elapsedTotal >= durationSeconds) {
            break;
        }
        
        totalFrames++;
        timer.waitForFrameEnd();
    }
    
    auto endTime = std::chrono::steady_clock::now();
    double totalTime = std::chrono::duration<double>(endTime - startTime).count();
    double averageFps = totalFrames / totalTime;
    
    std::cout << "\nTest Summary for 60Hz:" << std::endl;
    std::cout << "Total frames: " << totalFrames << std::endl;
    std::cout << "Total time: " << std::fixed << std::setprecision(3) << totalTime << "s" << std::endl;
    std::cout << "Average FPS: " << std::fixed << std::setprecision(1) << averageFps << std::endl;
    std::cout << "Average frame time: " << std::fixed << std::setprecision(3) 
              << timer.getAverageFrameTime() * 1000.0 << "ms" << std::endl;

    // Add some actual test assertions
    EXPECT_NEAR(totalTime, 5.0, 0.1);  // Test should run for ~5 seconds
    EXPECT_NEAR(averageFps, 60.0, 2.0);  // Average FPS should be close to 60
    EXPECT_NEAR(timer.getAverageFrameTime() * 1000.0, 16.67, 2.0);  // Frame time should be close to 16.67ms
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
} 