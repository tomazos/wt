#pragma once

#include "experimental/sdl/opengl.h"

namespace gl {

const char* ErrorToString(GLenum error);

void ThrowIfError();

[[noreturn]] void ThrowError();

}  // namespace gl
