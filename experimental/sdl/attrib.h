#pragma once

#include "experimental/sdl/opengl.h"

namespace gl {

class Attrib {
 public:
 private:
  Attrib(const string& name, GLuint location);

  string name_;
  GLuint location_;
  friend class Program;
  friend class VertexArray;
};

}  // namespace gl
