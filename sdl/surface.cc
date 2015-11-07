#include "sdl/surface.h"

#include <map>

#include "core/must.h"
#include "sdl/palette.h"

namespace sdl {

Surface::Surface(SDL_Surface* surface) : surface_(surface, SDL_FreeSurface) {
  if (!surface_) FAIL(SDL_GetError());
}

Surface::Surface(const filesystem::path& bmp_file)
    : Surface(SDL_LoadBMP(bmp_file.string().c_str())) {}

Surface::Surface(int width, int height, int depth, Uint32 Rmask, Uint32 Gmask,
                 Uint32 Bmask, Uint32 Amask)
    : Surface(SDL_CreateRGBSurface(0, width, height, depth, Rmask, Gmask, Bmask,
                                   Amask)) {}

Surface::Surface(void* pixels, int width, int height, int depth, int pitch,
                 Uint32 Rmask, Uint32 Gmask, Uint32 Bmask, Uint32 Amask)
    : Surface(SDL_CreateRGBSurfaceFrom(pixels, width, height, depth, pitch,
                                       Rmask, Gmask, Bmask, Amask)) {}

void Surface::SetPalette(Palette& palette) {
  if (SDL_SetSurfacePalette(surface_.get(), palette.palette_.get()) != 0)
    FAIL(SDL_GetError());
}

}  // namespace sdl
