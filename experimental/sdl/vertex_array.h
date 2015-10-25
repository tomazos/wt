#pragma once

#include "experimental/sdl/attrib.h"
#include "experimental/sdl/buffer.h"
#include "experimental/sdl/object.h"
#include "experimental/sdl/opengl.h"

namespace gl {

class VertexArray {
 public:
  VertexArray();

  // GL_POINTS, GL_LINE_STRIP, GL_LINE_LOOP, GL_LINES, GL_LINE_STRIP_ADJACENCY,
  // GL_LINES_ADJACENCY, GL_TRIANGLE_STRIP,
  // GL_TRIANGLE_FAN, GL_TRIANGLES, GL_TRIANGLE_STRIP_ADJACENCY,
  // GL_TRIANGLES_ADJACENCY and GL_PATCHES.
  void Draw(GLenum mode, GLint first, GLsizei count);

  void EnableAttribArray(const Attrib& attrib);
  void DisableAttribArray(const Attrib& attrib);

  void BindAttrib(const Attrib& attrib, GLuint bindingindex);

  void BindBuffer(GLuint bindingindex, const Buffer& buffer, GLintptr offset,
                  GLintptr stride);

  void AttribFormat(Attrib& attrib, GLint size, GLenum type,
                    GLboolean normalized, GLuint relativeoffset);

  void AttribIFormat(Attrib& attrib, GLint size, GLenum type,
                     GLuint relativeoffset);

  void AttribLFormat(Attrib& attrib, GLint size, GLenum type,
                     GLuint relativeoffset);

 private:
  void Bind();

  Object array_;
};

}  // namespace gl
