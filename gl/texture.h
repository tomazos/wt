#pragma once

#include "gl/object.h"
#include "gl/opengl.h"

namespace gl {

class Texture {
 public:
  Texture();

  static void Active(GLenum texture_unit);
  void Bind(GLenum target);

  //  static void Parameter(GLenum pname, float32 param);
  //  static void Parameter(GLenum pname, int param);
  //  static void Parameter(GLenum pname, )
  static void Image2D(GLenum target, GLint level, GLint internalformat,
                      GLsizei width, GLsizei height, GLenum format, GLenum type,
                      const void *pixels);

  // TODO private:
  Object texture_;
};

}  // namespace gl
