#pragma once

#include <SDL2/SDL.h>

#include "gl/geometry.h"
#include "sdl/renderer.h"
#include "sdl/surface.h"

namespace sdl {

class Texture {
 public:
  Texture(Renderer& renderer, Surface& surface);
  Texture(Renderer& renderer, uint32 format, int access, int w, int h);

  Rect Dimensions();

 private:
  Texture(SDL_Texture* texture);
  std::unique_ptr<SDL_Texture, void (*)(SDL_Texture*)> texture_;

  friend class Renderer;
};

}  // namespace sdl
