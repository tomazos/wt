#include "experimental/sdl/window.h"

#include "core/must.h"

namespace sdl {

Window::Window(const char* title, int x, int y, int w, int h, Uint32 flags)
    : window_(SDL_CreateWindow(title, x, y, w, h, flags), SDL_DestroyWindow) {
  MUST(window_);
}

void Window::Swap() { SDL_GL_SwapWindow(window_.get()); }

}  // namespace sdl
