#pragma once

#include <boost/filesystem.hpp>
#include <SDL2/SDL.h>

namespace sdl {

class Palette;

class Surface {
 public:
  Surface(const filesystem::path& bmp);
  Surface(int width, int height, int depth, Uint32 Rmask, Uint32 Gmask,
          Uint32 Bmask, Uint32 Amask);

  Surface(void* pixels, int width, int height, int depth, int pitch,
          Uint32 Rmask, Uint32 Gmask, Uint32 Bmask, Uint32 Amask);

  const SDL_PixelFormat& format() { return *surface_->format; }
  int pitch() { return surface_->pitch; }

  void* pixels() { return surface_->pixels; }

  int width() { return surface_->w; }
  int height() { return surface_->h; }

  void SetPalette(Palette& palette);

 private:
  Surface(SDL_Surface* surface);

  std::unique_ptr<SDL_Surface, void (*)(SDL_Surface*)> surface_;

  friend class Texture;
};

}  // namespace sdl
