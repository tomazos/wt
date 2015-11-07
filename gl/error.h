#pragma once

#include "gl/opengl.h"

namespace gl {

const char* ErrorToString(GLenum error);

void ThrowIfError();

[[noreturn]] void ThrowError();

}  // namespace gl
