#pragma once

#include <SDL3/SDL.h>
#include <array>

/**
 * Timer class for managing frame timing and ensuring consistent frame rates.
 * 
 * Features:
 * - High-precision timing using SDL_GetTicks64
 * - Frame rate limiting with sleep and busy-waiting
 * - FPS tracking and calculation
 * - Frame time history for smoothing
 * - Automatic cleanup of old frame time entries
 */
class Timer {
public:
    /**
     * Initialize the Timer with a target frame rate.
     * 
     * @param targetFps Desired frames per second (default: 60)
     */
    explicit Timer(int targetFps = 60);

    /**
     * Record the start time of the current frame.
     * Should be called at the beginning of each frame.
     */
    void startFrame();

    /**
     * Sleep for the remaining time to maintain target frame rate.
     * Uses a combination of SDL_Delay and busy-waiting for precision.
     */
    void waitForFrameEnd();

    /**
     * Get the time elapsed since frame start in seconds.
     * 
     * @return Elapsed time in seconds
     */
    double getElapsedTime() const;

    /**
     * Get the current frames per second.
     * 
     * @return Current FPS as an integer
     */
    int getFps() const;

    /**
     * Get the average frame time over the last 60 frames.
     * 
     * @return Average frame time in seconds
     */
    double getAverageFrameTime() const;

    /**
     * Set the target frame rate.
     * 
     * @param fps Desired frames per second
     */
    void setTargetFps(int fps);

    /**
     * Get the current game time in seconds since Timer creation.
     * 
     * This provides a hardware-independent, monotonic time source for game systems.
     * Use this instead of direct hardware timing functions like SDL_GetTicks() or
     * std::chrono calls to ensure consistent, testable timing behavior.
     * 
     * @return Time in seconds since Timer creation
     * 
     * @example
     * // System timing with hardware independence
     * if (timer->getClock() - lastAction > cooldown) {
     *     performAction();
     *     lastAction = timer->getClock();
     * }
     */
    double getClock() const;

private:
    static constexpr int MAX_FRAME_HISTORY = 60;  // Keep last 60 frames for smoothing
    static constexpr double FPS_UPDATE_INTERVAL = 1.0;  // Update FPS every second

    Uint32 creationTicks;          // Track timer creation time for getClock()
    Uint32 frameStartTicks;        // Frame start time in ticks
    double targetFrameTime;        // Target time per frame in seconds
    int frames;                    // Frame counter for FPS calculation
    int currentFps;               // Current FPS value
    Uint32 lastFpsUpdate;         // Last time FPS was updated
    double sleepError;            // Track sleep inaccuracy
    double lastFrameTime;         // Actual elapsed time of the last frame

    // Circular buffer for frame times
    std::array<double, MAX_FRAME_HISTORY> frameTimes;
    int frameTimeIndex;           // Current position in circular buffer
    int frameTimeCount;           // Number of valid entries in buffer
}; 