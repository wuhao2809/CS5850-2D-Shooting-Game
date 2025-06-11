#pragma once

#include <SDL3/SDL.h>

namespace game {
struct GameColor {
  Uint8 r;
  Uint8 g;
  Uint8 b;
  Uint8 a;

  GameColor(Uint8 red, Uint8 green, Uint8 blue, Uint8 alpha = 255)
      : r(red), g(green), b(blue), a(alpha) {}
};
} // namespace game
