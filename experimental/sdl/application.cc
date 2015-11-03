#include "experimental/sdl/application.h"

#include <SDL2/SDL.h>

#include "core/must.h"

namespace sdl {

Application::Application() {
  if (SDL_Init(SDL_INIT_VIDEO) != 0) FAIL(SDL_GetError());
}

Application::~Application() { SDL_Quit(); }

}  // namespace sdl
