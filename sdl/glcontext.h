#pragma once

#include "sdl/window.h"

namespace sdl {

class GLContext {
 public:
  GLContext(Window& window);

  ~GLContext();

 private:
  SDL_GLContext context_;
};

}  // namespace sdl
