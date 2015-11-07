#include "sdl/renderer.h"

#include <map>

#include "core/must.h"
#include "sdl/pixel_format.h"
#include "sdl/texture.h"

namespace sdl {

Renderer::Renderer(Window& window)
    : renderer_(
          SDL_CreateRenderer(window.window_.get(), -1 /*index*/, 0 /*flags*/),
          SDL_DestroyRenderer) {
  MUST(renderer_);
}

Rect Renderer::GetViewport() {
  Rect viewport;
  SDL_RenderGetViewport(renderer_.get(), &viewport);
  return viewport;
}

void Renderer::Clear() {
  if (SDL_RenderClear(renderer_.get()) != 0) FAIL(SDL_GetError());
}

void Renderer::SetDrawColor(gl::vec4 rgba) {
  if (SDL_SetRenderDrawColor(renderer_.get(), rgba(0) * 255, rgba(1) * 255,
                             rgba(2) * 255, rgba(3) * 255) != 0)
    FAIL(SDL_GetError());
}

void Renderer::DrawLine(int x1, int y1, int x2, int y2) {
  if (SDL_RenderDrawLine(renderer_.get(), x1, y1, x2, y2) != 0)
    FAIL(SDL_GetError());
}

void Renderer::Render(Texture& texture, const optional<Rect>& src,
                      const optional<Rect>& dest) {
  if (SDL_RenderCopy(renderer_.get(), texture.texture_.get(),
                     src ? &*src : nullptr, dest ? &*dest : nullptr) != 0)
    FAIL(SDL_GetError());
}

void Renderer::Present() { SDL_RenderPresent(renderer_.get()); }

void Renderer::LogDriversInfo() {
  LOGEXPR(SDL_GetNumRenderDrivers());

  for (int i = 0; i < SDL_GetNumRenderDrivers(); ++i) {
    LOGEXPR(i);
    SDL_RendererInfo info;
    memset(&info, 0, sizeof info);
    if (SDL_GetRenderDriverInfo(i, &info) != 0) FAIL(SDL_GetError());
    LOGEXPR(info.name);
    LOGEXPR(info.flags & SDL_RENDERER_SOFTWARE);
    LOGEXPR(info.flags & SDL_RENDERER_ACCELERATED);
    LOGEXPR(info.flags & SDL_RENDERER_PRESENTVSYNC);
    LOGEXPR(info.flags & SDL_RENDERER_TARGETTEXTURE);
    LOGEXPR(info.max_texture_width);
    LOGEXPR(info.max_texture_height);
    LOGEXPR(info.num_texture_formats);
    for (size_t j = 0; j < info.num_texture_formats; ++j) {
      uint32 format = info.texture_formats[j];
      string_view format_name = PixelFormatEnumToString(format);
      LOGEXPR(format_name);
    }
  }
}

}  // namespace sdl
