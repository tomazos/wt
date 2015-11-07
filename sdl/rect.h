#pragma once

#include <SDL2/SDL.h>

namespace sdl {

using Rect = SDL_Rect;

inline Rect operator&(const Rect& a, const Rect& b) {
  Rect c;
  SDL_UnionRect(&a, &b, &c);
  return c;
}

}  // namespace sdl
