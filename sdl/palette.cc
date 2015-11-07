#include "sdl/palette.h"

#include "core/must.h"

namespace sdl {

Palette::Palette(int ncolors)
    : palette_(SDL_AllocPalette(ncolors), SDL_FreePalette) {
  MUST(palette_);
}

void Palette::SetColor(int ncolor, gl::vec4 color) {
  SDL_Color sdl_color{uint8(color(0) * 255), uint8(color(1) * 255),
                      uint8(color(2) * 255), uint8(color(3) * 255)};

  if (SDL_SetPaletteColors(palette_.get(), &sdl_color, ncolor, 1) != 0)
    FAIL(SDL_GetError());
}

}  // namespace sdl
