#pragma once

#include <SDL2/SDL.h>

namespace sdl {

class Window {
 public:
  Window(const char* title, int x, int y, int w, int h, Uint32 flags);

  void Swap();

  SDL_DisplayMode GetDisplayMode();

 private:
  std::unique_ptr<SDL_Window, void (*)(SDL_Window*)> window_;

  friend class GLContext;
  friend class Renderer;
};

}  // namespace sdl
