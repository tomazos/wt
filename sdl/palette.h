#pragma once

#include <SDL2/SDL.h>

#include "gl/geometry.h"

namespace sdl {

class Palette {
 public:
  Palette(int ncolors);

  void SetColor(int ncolor, gl::vec4 color);

 private:
  std::unique_ptr<SDL_Palette, void (*)(SDL_Palette*)> palette_;

  friend class Surface;
  friend class Renderer;
};

}  // namespace sdl
