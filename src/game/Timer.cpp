#include "Timer.hpp"
#include <algorithm>
#include <numeric>

Timer::Timer(int targetFps)
    : frameStartTicks(0)
    , targetFrameTime(1.0 / targetFps)
    , frames(0)
    , currentFps(targetFps)
    , lastFpsUpdate(0)
    , sleepError(0.0)
    , frameTimeIndex(0)
    , frameTimeCount(0)
    , lastFrameTime(0.0)
{
    // Initialize frame times with target frame time
    frameTimes.fill(targetFrameTime);
}

void Timer::startFrame() {
    frameStartTicks = SDL_GetTicks();
    frames++;

    // Update FPS counter every second
    Uint64 currentTime = frameStartTicks;
    double timeSinceLastUpdate = (currentTime - lastFpsUpdate) / 1000.0;
    
    if (timeSinceLastUpdate >= FPS_UPDATE_INTERVAL) {
        currentFps = static_cast<int>(frames / timeSinceLastUpdate + 0.5);  // Round to nearest integer
        frames = 0;
        lastFpsUpdate = currentTime;
    }
}

void Timer::waitForFrameEnd() {
    Uint64 currentTime = SDL_GetTicks();
    double elapsed = (currentTime - frameStartTicks) / 1000.0;
    double remainingTime = targetFrameTime - elapsed - sleepError;

    if (remainingTime > 0) {
        // Sleep for 90% of the remaining time
        if (remainingTime > 0.001) {  // Only sleep if more than 1ms remaining
            Uint32 sleepMs = static_cast<Uint32>((remainingTime * 0.9) * 1000);
            if (sleepMs > 0) {
                SDL_Delay(sleepMs);
            }
        }

        // Busy wait for the rest of the time
        while ((SDL_GetTicks() - frameStartTicks) / 1000.0 < targetFrameTime) {
            SDL_Delay(0);  // Yield to other threads
        }

        // Update sleep error tracking
        double actualSleep = (SDL_GetTicks() - currentTime) / 1000.0;
        sleepError = actualSleep - remainingTime;
    } else {
        sleepError = 0.0;  // Reset sleep error if we're running behind
    }

    // Update frame time history and store last frame time
    lastFrameTime = (SDL_GetTicks() - frameStartTicks) / 1000.0;
    frameTimes[frameTimeIndex] = lastFrameTime;
    frameTimeIndex = (frameTimeIndex + 1) % MAX_FRAME_HISTORY;
    frameTimeCount = std::min(frameTimeCount + 1, MAX_FRAME_HISTORY);
}

double Timer::getElapsedTime() const {
    return lastFrameTime;
}

int Timer::getFps() const {
    return currentFps;
}

double Timer::getAverageFrameTime() const {
    if (frameTimeCount == 0) return targetFrameTime;

    double sum = std::accumulate(frameTimes.begin(), 
                               frameTimes.begin() + frameTimeCount, 
                               0.0);
    return sum / frameTimeCount;
}

void Timer::setTargetFps(int fps) {
    targetFrameTime = 1.0 / fps;
} 