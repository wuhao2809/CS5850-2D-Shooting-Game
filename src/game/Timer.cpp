#include "Timer.hpp"
#include <SDL3/SDL.h>
#include <algorithm>

Timer::Timer(int targetFps)
    : frameStartTicks(SDL_GetPerformanceCounter()),
      targetFrameTime(1.0 / targetFps), frames(0), currentFps(targetFps),
      lastFpsUpdate(SDL_GetPerformanceCounter()), sleepError(0.0),
      frameTimeIndex(0), frameTimeCount(0)
{
  frameTimes.fill()
}

void Timer::startFrame()
{
  frameStartTicks = SDL_GetPerformanceCounter();
  frames++;

  Uint64 currentTime = frameStartTicks;
  double timeSinceLastUpdate = (currentTime - LastFpsUpdate) / (double)SDL_GetPerformanceFrequency();

  if (timeSinceLastUpdate >= FPS_UPDATE_INTERVAL) {
    currentFps = static_cast<int>(frames / timeSinceLastUpdate + 0.5)
  }
}
