#include <gtest/gtest.h>
#include <chrono>
#include <thread>
#include <vector>
#include <algorithm>
#include "../game/Timer.hpp"

/**
 * Timer Test Suite - Core Functionality
 * 
 * Tests the Timer class core API and getClock() functionality
 * without component integration tests.
 * 
 * Based on Python (17 tests) and Java (19 tests) implementations.
 */
class TimerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create fresh Timer for each test
        timer = std::make_unique<Timer>(60);
    }
    
    void TearDown() override {
        timer.reset();
    }
    
    std::unique_ptr<Timer> timer;
    
    // Helper function to measure timing accuracy
    double measureSleepAccuracy(int milliseconds) {
        double start = timer->getClock();
        std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
        double end = timer->getClock();
        return end - start;
    }
};

// ===================================================================
// Core Timer API Tests (6 tests)
// ===================================================================

TEST_F(TimerTest, ConstructorDefaultFps) {
    Timer defaultTimer;
    EXPECT_EQ(defaultTimer.getFps(), 60);
    
    // Should start with reasonable initial values
    EXPECT_GE(defaultTimer.getAverageFrameTime(), 0.0);
    EXPECT_LT(defaultTimer.getClock(), 0.001); // Should start near zero
}

TEST_F(TimerTest, ConstructorCustomFps) {
    Timer customTimer(30);
    EXPECT_EQ(customTimer.getFps(), 30);
    
    Timer highFpsTimer(120);
    EXPECT_EQ(highFpsTimer.getFps(), 120);
    
    // getClock should work for all frame rates
    EXPECT_GE(customTimer.getClock(), 0.0);
    EXPECT_GE(highFpsTimer.getClock(), 0.0);
}

TEST_F(TimerTest, FrameTimingPrecision) {
    const int NUM_FRAMES = 10;
    std::vector<double> frameTimes;
    
    for (int i = 0; i < NUM_FRAMES; ++i) {
        timer->startFrame();
        
        // Simulate some work (2ms)
        std::this_thread::sleep_for(std::chrono::milliseconds(2));
        
        timer->waitForFrameEnd();
        frameTimes.push_back(timer->getElapsedTime());
    }
    
    // Check that frame times are reasonable (within 50ms tolerance for test environment)
    double targetFrameTime = 1.0 / 60.0; // ~16.67ms
    for (double frameTime : frameTimes) {
        EXPECT_GE(frameTime, targetFrameTime - 0.05); // At least target-50ms
        EXPECT_LE(frameTime, targetFrameTime + 0.05); // At most target+50ms
    }
}

TEST_F(TimerTest, FpsCalculationAccuracy) {
    const int TARGET_FPS = 30;
    timer = std::make_unique<Timer>(TARGET_FPS);
    
    // Run several frames to let FPS calculation stabilize
    for (int i = 0; i < 10; ++i) {
        timer->startFrame();
        std::this_thread::sleep_for(std::chrono::milliseconds(5)); // Light work
        timer->waitForFrameEnd();
    }
    
    int measuredFps = timer->getFps();
    
    // FPS should be within reasonable range (allow for test environment variation)
    EXPECT_GE(measuredFps, TARGET_FPS - 10);
    EXPECT_LE(measuredFps, TARGET_FPS + 10);
}

TEST_F(TimerTest, PerformanceUnderLoad) {
    // Test Timer behavior under CPU load
    timer = std::make_unique<Timer>(60);
    
    std::vector<double> clockReadings;
    const int NUM_READINGS = 100;
    
    for (int i = 0; i < NUM_READINGS; ++i) {
        // Simulate CPU work
        volatile int dummy = 0;
        for (int j = 0; j < 1000; ++j) {
            dummy += j;
        }
        
        clockReadings.push_back(timer->getClock());
    }
    
    // Verify monotonic property is maintained under load
    for (size_t i = 1; i < clockReadings.size(); ++i) {
        EXPECT_GE(clockReadings[i], clockReadings[i-1]) 
            << "Time should be monotonic even under CPU load at reading " << i;
    }
}

TEST_F(TimerTest, AverageFrameTimeCalculation) {
    const int NUM_FRAMES = 5;
    const int WORK_MS = 10;
    
    // Run consistent frames with known work duration
    for (int i = 0; i < NUM_FRAMES; ++i) {
        timer->startFrame();
        std::this_thread::sleep_for(std::chrono::milliseconds(WORK_MS));
        timer->waitForFrameEnd();
    }
    
    double avgFrameTime = timer->getAverageFrameTime();
    
    // Average should be reasonable (accounting for frame rate control)
    EXPECT_GE(avgFrameTime, WORK_MS / 1000.0); // At least work duration
    EXPECT_LE(avgFrameTime, 0.1); // Should not exceed 100ms
}

// ===================================================================
// getClock() API Tests (6 tests)
// ===================================================================

TEST_F(TimerTest, GetClockMonotonicProperty) {
    std::vector<double> readings;
    const int NUM_READINGS = 100;
    
    for (int i = 0; i < NUM_READINGS; ++i) {
        readings.push_back(timer->getClock());
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    
    // Verify monotonic property - time should never go backward
    for (size_t i = 1; i < readings.size(); ++i) {
        EXPECT_GE(readings[i], readings[i-1]) 
            << "Time should never go backward at reading " << i
            << " (current: " << readings[i] << ", previous: " << readings[i-1] << ")";
    }
}

TEST_F(TimerTest, GetClockPrecision) {
    const int SLEEP_MS = 100;
    double expectedDuration = SLEEP_MS / 1000.0; // Convert to seconds
    
    double actualDuration = measureSleepAccuracy(SLEEP_MS);
    
    // Should be accurate within 20ms (reasonable for test environment)
    EXPECT_NEAR(expectedDuration, actualDuration, 0.02)
        << "getClock() should have reasonable precision. Expected: " << expectedDuration 
        << "s, Actual: " << actualDuration << "s";
}

TEST_F(TimerTest, GetClockZeroStart) {
    double initialClock = timer->getClock();
    
    // Should be very close to zero (within 2ms due to SDL_GetTicks millisecond precision)
    EXPECT_LE(initialClock, 0.002) 
        << "getClock() should start near zero, got: " << initialClock;
    EXPECT_GE(initialClock, 0.0) 
        << "getClock() should never be negative, got: " << initialClock;
}

TEST_F(TimerTest, GetClockIndependenceFromFrameOps) {
    std::vector<double> clockReadings;
    
    for (int i = 0; i < 5; ++i) {
        clockReadings.push_back(timer->getClock());
        
        timer->startFrame();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        timer->waitForFrameEnd();
        
        clockReadings.push_back(timer->getClock());
    }
    
    // Verify all readings are monotonic regardless of frame operations
    // Note: SDL_GetTicks() has millisecond precision, so consecutive readings may be equal
    for (size_t i = 1; i < clockReadings.size(); ++i) {
        EXPECT_GE(clockReadings[i], clockReadings[i-1])
            << "getClock() should be monotonic (never go backward) at reading " << i;
    }
}

TEST_F(TimerTest, GetClockFpsIndependence) {
    // Test that getClock() works consistently across different FPS settings
    Timer timer30(30);
    Timer timer60(60);
    Timer timer120(120);
    
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    
    double time30 = timer30.getClock();
    double time60 = timer60.getClock();
    double time120 = timer120.getClock();
    
    // All should report similar elapsed times regardless of target FPS
    EXPECT_NEAR(time30, time60, 0.01) 
        << "getClock() should be FPS-independent (30 vs 60 FPS)";
    EXPECT_NEAR(time60, time120, 0.01) 
        << "getClock() should be FPS-independent (60 vs 120 FPS)";
    
    // All should be positive and reasonable
    EXPECT_GT(time30, 0.0);
    EXPECT_GT(time60, 0.0);
    EXPECT_GT(time120, 0.0);
    EXPECT_LT(time30, 1.0); // Should be less than 1 second for this test
}

TEST_F(TimerTest, GetClockSystemTimingPatterns) {
    // Test common game timing patterns using getClock()
    
    // Pattern 1: Cooldown timing
    double fireCooldown = 0.1; // 100ms cooldown
    double lastFireTime = timer->getClock();
    
    // Should not be able to "fire" immediately
    EXPECT_FALSE(timer->getClock() - lastFireTime >= fireCooldown)
        << "Cooldown should not be ready immediately";
    
    // Wait for cooldown
    std::this_thread::sleep_for(std::chrono::milliseconds(120));
    
    // Should be able to "fire" now
    EXPECT_TRUE(timer->getClock() - lastFireTime >= fireCooldown)
        << "Cooldown should be ready after waiting";
    
    // Pattern 2: Spawn timing
    double spawnInterval = 0.05; // 50ms spawn interval
    double lastSpawnTime = timer->getClock();
    int spawnCount = 0;
    
    // Count spawns over time
    double testDuration = 0.2; // 200ms test
    double startTime = timer->getClock();
    
    while (timer->getClock() - startTime < testDuration) {
        if (timer->getClock() - lastSpawnTime >= spawnInterval) {
            spawnCount++;
            lastSpawnTime = timer->getClock();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    
    // Should have spawned approximately the right number of times
    int expectedSpawns = static_cast<int>(testDuration / spawnInterval);
    EXPECT_GE(spawnCount, expectedSpawns - 2) 
        << "Should spawn approximately correct number of times";
    EXPECT_LE(spawnCount, expectedSpawns + 2) 
        << "Should not spawn too many times";
}

// ===================================================================
// Design Philosophy Tests (3 tests - focused on core functionality)
// ===================================================================

TEST_F(TimerTest, FrameTimeHistoryManagement) {
    // Test that Timer properly manages frame time history
    
    // Do several frames with varying work loads
    std::vector<int> workLoads = {5, 10, 15, 8, 12}; // milliseconds
    
    for (int workMs : workLoads) {
        timer->startFrame();
        std::this_thread::sleep_for(std::chrono::milliseconds(workMs));
        timer->waitForFrameEnd();
    }
    
    double avgFrameTime = timer->getAverageFrameTime();
    
    // Average should be reasonable and reflect the varying workloads
    EXPECT_GT(avgFrameTime, 0.005) << "Average frame time should be positive";
    EXPECT_LT(avgFrameTime, 0.1) << "Average frame time should be reasonable";
}

TEST_F(TimerTest, SleepErrorCompensation) {
    // Test that Timer handles sleep inaccuracies gracefully
    timer = std::make_unique<Timer>(60); // 60 FPS = ~16.67ms per frame
    
    std::vector<double> frameTimes;
    const int NUM_FRAMES = 10;
    
    for (int i = 0; i < NUM_FRAMES; ++i) {
        timer->startFrame();
        
        // Simulate light work
        std::this_thread::sleep_for(std::chrono::milliseconds(2));
        
        timer->waitForFrameEnd();
        frameTimes.push_back(timer->getElapsedTime());
    }
    
    // Frame times should converge toward target (compensation working)
    double targetFrameTime = 1.0 / 60.0;
    double lastFrameTime = frameTimes.back();
    
    // Final frame should be closer to target than a naive approach
    EXPECT_LT(std::abs(lastFrameTime - targetFrameTime), 0.05)
        << "Sleep error compensation should improve timing accuracy";
}

TEST_F(TimerTest, HardwareIndependenceValidation) {
    // Test that getClock() provides hardware-independent timing
    
    // Multiple Timer instances should provide consistent relative timing
    Timer timer1(60);
    Timer timer2(60);
    
    double start1 = timer1.getClock();
    double start2 = timer2.getClock();
    
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    double end1 = timer1.getClock();
    double end2 = timer2.getClock();
    
    double duration1 = end1 - start1;
    double duration2 = end2 - start2;
    
    // Both should measure similar durations (hardware-independent)
    EXPECT_NEAR(duration1, duration2, 0.01)
        << "Independent Timer instances should measure consistent durations";
    
    // Both should be positive and reasonable
    EXPECT_GT(duration1, 0.08) << "Duration should be at least 80ms";
    EXPECT_LT(duration1, 0.15) << "Duration should be at most 150ms";
}

// ===================================================================
// Performance and Edge Case Tests (2 tests)
// ===================================================================

TEST_F(TimerTest, HighFrequencyClockReads) {
    // Test getClock() performance under high-frequency calls
    const int NUM_CALLS = 10000;
    std::vector<double> readings;
    readings.reserve(NUM_CALLS);
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < NUM_CALLS; ++i) {
        readings.push_back(timer->getClock());
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    
    // Performance check: should complete in reasonable time
    EXPECT_LT(duration.count(), 100000) // Less than 100ms for 10k calls
        << "getClock() should be efficient for high-frequency calls";
    
    // Verify monotonic property maintained at high frequency
    for (size_t i = 1; i < readings.size(); ++i) {
        EXPECT_GE(readings[i], readings[i-1])
            << "Monotonic property should hold even at high frequency";
    }
}

TEST_F(TimerTest, LongRunningTimerAccuracy) {
    // Test that Timer maintains accuracy over longer periods
    timer = std::make_unique<Timer>(60);
    
    const double TEST_DURATION = 1.0; // 1 second test
    double startTime = timer->getClock();
    
    // Simulate long-running operation with periodic checks
    while (timer->getClock() - startTime < TEST_DURATION) {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    
    double endTime = timer->getClock();
    double measuredDuration = endTime - startTime;
    
    // Should be accurate within 50ms over 1 second
    EXPECT_NEAR(TEST_DURATION, measuredDuration, 0.05)
        << "Timer should maintain accuracy over longer periods";
} 