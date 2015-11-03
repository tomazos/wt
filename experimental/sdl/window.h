#pragma once

#include <SDL2/SDL.h>

namespace sdl {

struct Window {
 public:
  Window(const char* title, int x, int y, int w, int h, Uint32 flags);

  void Swap();

 private:
  std::unique_ptr<SDL_Window, void (*)(SDL_Window*)> window_;

  friend class GLContext;
};

}  // namespace sdl
