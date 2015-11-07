#pragma once

#include <SDL2/SDL.h>

namespace sdl {

using PixelFormat = SDL_PixelFormat;
using PixelFormatEnum = uint32;

string_view PixelFormatEnumToString(PixelFormatEnum pixel_format_enum);

}  // namespace sdl
