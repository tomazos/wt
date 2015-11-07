#include "sdl/window.h"

#include "core/must.h"

namespace sdl {

Window::Window(const char* title, int x, int y, int w, int h, Uint32 flags)
    : window_(SDL_CreateWindow(title, x, y, w, h, flags), SDL_DestroyWindow) {
  MUST(window_);
}

void Window::Swap() { SDL_GL_SwapWindow(window_.get()); }

SDL_DisplayMode Window::GetDisplayMode() {
  SDL_DisplayMode mode;
  std::memset(&mode, 0, sizeof mode);
  if (SDL_GetWindowDisplayMode(window_.get(), &mode) != 0) FAIL(SDL_GetError());
  return mode;
}

}  // namespace sdl
