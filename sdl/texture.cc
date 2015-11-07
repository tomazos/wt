#include "sdl/texture.h"

#include <map>

#include "core/must.h"

namespace sdl {

Texture::Texture(Renderer& renderer, Surface& surface)
    : Texture(SDL_CreateTextureFromSurface(renderer.renderer_.get(),
                                           surface.surface_.get())) {}

Texture::Texture(Renderer& renderer, uint32 format, int access, int w, int h)
    : Texture(
          SDL_CreateTexture(renderer.renderer_.get(), format, access, w, h)) {}

Texture::Texture(SDL_Texture* texture) : texture_(texture, SDL_DestroyTexture) {
  MUST(texture_);
}

Rect Texture::Dimensions() {
  Rect rect{0, 0, 0, 0};
  Uint32 format;
  int access;
  if (SDL_QueryTexture(texture_.get(), &format, &access, &rect.w, &rect.h) != 0)
    FAIL(SDL_GetError());
  return rect;
}

}  // namespace sdl
