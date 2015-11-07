#pragma once

#include "gl/object.h"
#include "gl/opengl.h"

namespace gl {

class Shader {
 public:
  Shader();

  // shader_type is one of GL_COMPUTE_SHADER, GL_VERTEX_SHADER,
  // GL_TESS_CONTROL_SHADER, GL_TESS_EVALUATION_SHADER, GL_GEOMETRY_SHADER,
  // GL_FRAGMENT_SHADER.
  explicit Shader(GLenum shader_type);
  Shader(GLenum shader_type, string_view source);

  string GetInfoLog() const;

  void Source(string_view source);

  void Compile();

 private:
  GLint Get(GLenum pname) const;

  Object shader_;

  friend class Program;
};

}  // namespace gl
