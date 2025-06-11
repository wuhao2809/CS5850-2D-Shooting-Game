#pragma once

#include <SDL3/SDL.h>

namespace game {
class GameColor {
public:
  GameColor() : r(0), g(0), b(0), a(255) {}
  GameColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255)
      : r(r), g(g), b(b), a(a) {}

  operator SDL_Color() const { return {r, g, b, a}; }

  static const GameColor BACKGROUND; // White background

  Uint8 r, g, b, a;
};

inline const GameColor GameColor::BACKGROUND(255, 255, 255, 255);
} // namespace game