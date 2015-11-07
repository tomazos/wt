#include "gl/shader.h"

#include "core/must.h"
#include "gl/error.h"

namespace gl {

Shader::Shader() : shader_(glDeleteShader) {}

Shader::Shader(GLenum shader_type)
    : shader_(glCreateShader(shader_type), glDeleteShader) {}

Shader::Shader(GLenum shader_type, string_view source) : Shader(shader_type) {
  Source(source);
  Compile();
}

string Shader::GetInfoLog() const {
  const GLsizei max_length = Get(GL_INFO_LOG_LENGTH);
  string info_log(max_length - 1, '\0');
  GLsizei length = -1;
  glGetShaderInfoLog(shader_, max_length, &length, &info_log[0]);
  ThrowIfError();
  MUST_EQ(length + 1, max_length);
  return info_log;
}

void Shader::Source(string_view source) {
  const GLchar* string = source.data();
  GLint length = source.size();
  glShaderSource(shader_, 1 /*count*/, &string, &length);
  ThrowIfError();
}

void Shader::Compile() {
  glCompileShader(shader_);
  ThrowIfError();
  if (Get(GL_COMPILE_STATUS) != GL_TRUE) FAIL(GetInfoLog());
}

GLint Shader::Get(GLenum pname) const {
  GLint params;
  glGetShaderiv(shader_, pname, &params);
  ThrowIfError();
  return params;
}

}  // namespace gl
