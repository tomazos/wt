#include "gl/error.h"

namespace gl {

const char* ErrorToString(GLenum error) {
  switch (error) {
    case GL_NO_ERROR:
      return "no error";

    case GL_INVALID_ENUM:
      return "invalid enum";

    case GL_INVALID_VALUE:
      return "invalid value";

    case GL_INVALID_OPERATION:
      return "invalid operation";

    case GL_INVALID_FRAMEBUFFER_OPERATION:
      return "invalid framebuffer operation";

    case GL_OUT_OF_MEMORY:
      return "out of memory";

    case GL_STACK_UNDERFLOW:
      return "stack underflow";

    case GL_STACK_OVERFLOW:
      return "stack overflow";

    default:
      return "unknown error";
  }
}

[[noreturn]] static void ThrowErrorString(GLenum error) {
  const char* error_string = ErrorToString(error);
  throw std::logic_error(error_string);
}

void ThrowIfError() {
  GLenum error = glGetError();
  if (error != GL_NO_ERROR) ThrowErrorString(error);
}

[[noreturn]] void ThrowError() {
  GLenum error = glGetError();
  ThrowErrorString(error);
}

}  // namespace gl
