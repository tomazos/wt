#include "experimental/sdl/glcontext.h"

#include "core/must.h"

namespace sdl {

GLContext::GLContext(Window& window)
    : context_(SDL_GL_CreateContext(window.window_.get())) {
  MUST(context_ != nullptr);
}

GLContext::~GLContext() { SDL_GL_DeleteContext(context_); }

}  // namespace sdl
