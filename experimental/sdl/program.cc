#include "experimental/sdl/program.h"

#include "core/must.h"
#include "experimental/sdl/error.h"

namespace gl {

Program::Program()
    : program_(glCreateProgram(), [](GLuint name) { glDeleteProgram(name); }) {}

string Program::GetInfoLog() const {
  const GLsizei max_length = Get(GL_INFO_LOG_LENGTH);
  string info_log(max_length - 1, '\0');
  GLsizei length = -1;
  glGetProgramInfoLog(program_, max_length, &length, &info_log[0]);
  ThrowIfError();
  MUST_EQ(length + 1, max_length);
  return info_log;
}

void Program::Attach(const Shader& shader) {
  glAttachShader(program_, shader.shader_);
  ThrowIfError();
}

void Program::Detach(const Shader& shader) {
  glAttachShader(program_, shader.shader_);
  ThrowIfError();
}

Attrib Program::BindAttrib(const string& name) {
  return {name, BindAttribLocation(name)};
}

void Program::Link() {
  glLinkProgram(program_);
  ThrowIfError();
  if (Get(GL_LINK_STATUS) != GL_TRUE) FAIL(GetInfoLog());
}

void Program::Use() {
  glUseProgram(program_);
  ThrowIfError();
}

GLuint Program::BindAttribLocation(const string& name) {
  const GLuint index = next_available_index++;
  BindAttribLocation(index, name);
  return index;
}

void Program::BindAttribLocation(GLuint index, const string& name) {
  glBindAttribLocation(program_, index, name.c_str());
  ThrowIfError();
}

GLint Program::GetAttribLocation(const string& name) const {
  GLint attrib_location = glGetAttribLocation(program_, name.c_str());
  ThrowIfError();
  if (attrib_location == -1) FAIL(name, "not found");
  return attrib_location;
}

GLint Program::GetUniformLocation(const string& name) const {
  GLint uniform_location = glGetUniformLocation(program_, name.c_str());
  ThrowIfError();
  if (uniform_location == -1) FAIL(name, " not found");
  return uniform_location;
}

template <typename F, typename... Args>
void Program::UniformImpl(F f, const string& name, Args&&... args) {
  f(program_, GetUniformLocation(name), std::forward<Args>(args)...);
  ThrowIfError();
}

void Program::Uniform(const string& name, bool value) {
  UniformImpl(glProgramUniform1i, name, value);
}

void Program::Uniform(const string& name, bvec2 value) {
  UniformImpl(glProgramUniform2i, name, value[0], value[1]);
}

void Program::Uniform(const string& name, bvec3 value) {
  UniformImpl(glProgramUniform3i, name, value[0], value[1], value[2]);
}

void Program::Uniform(const string& name, bvec4 value) {
  UniformImpl(glProgramUniform4i, name, value[0], value[1], value[2], value[3]);
}

void Program::Uniform(const string& name, int32 value) {
  UniformImpl(glProgramUniform1i, name, value);
}

void Program::Uniform(const string& name, ivec2 value) {
  UniformImpl(glProgramUniform2i, name, value[0], value[1]);
}

void Program::Uniform(const string& name, ivec3 value) {
  UniformImpl(glProgramUniform3i, name, value[0], value[1], value[2]);
}

void Program::Uniform(const string& name, ivec4 value) {
  UniformImpl(glProgramUniform4i, name, value[0], value[1], value[2], value[3]);
}

void Program::Uniform(const string& name, uint32 v0) {
  UniformImpl(glProgramUniform1ui, name, v0);
}

void Program::Uniform(const string& name, uvec2 value) {
  UniformImpl(glProgramUniform2ui, name, value[0], value[1]);
}

void Program::Uniform(const string& name, uvec3 value) {
  UniformImpl(glProgramUniform3ui, name, value[0], value[1], value[2]);
}

void Program::Uniform(const string& name, uvec4 value) {
  UniformImpl(glProgramUniform4ui, name, value[0], value[1], value[2],
              value[3]);
}

void Program::Uniform(const string& name, float32 v0) {
  UniformImpl(glProgramUniform1f, name, v0);
}

void Program::Uniform(const string& name, vec2 value) {
  UniformImpl(glProgramUniform2f, name, value[0], value[1]);
}

void Program::Uniform(const string& name, vec3 value) {
  UniformImpl(glProgramUniform3f, name, value[0], value[1], value[2]);
}

void Program::Uniform(const string& name, vec4 value) {
  UniformImpl(glProgramUniform4f, name, value[0], value[1], value[2], value[3]);
}

void Program::Uniform(const string& name, float64 v0) {
  UniformImpl(glProgramUniform1d, name, v0);
}

void Program::Uniform(const string& name, dvec2 value) {
  UniformImpl(glProgramUniform2d, name, value[0], value[1]);
}

void Program::Uniform(const string& name, dvec3 value) {
  UniformImpl(glProgramUniform3d, name, value[0], value[1], value[2]);
}

void Program::Uniform(const string& name, dvec4 value) {
  UniformImpl(glProgramUniform4d, name, value[0], value[1], value[2], value[3]);
}

void Program::Uniform(const string& name, mat2 value) {
  UniformImpl(glProgramUniformMatrix2fv, name, 1, GL_FALSE, value.data());
}
void Program::Uniform(const string& name, mat2x3 value) {
  UniformImpl(glProgramUniformMatrix2x3fv, name, 1, GL_FALSE, value.data());
}
void Program::Uniform(const string& name, mat2x4 value) {
  UniformImpl(glProgramUniformMatrix2x4fv, name, 1, GL_FALSE, value.data());
}
void Program::Uniform(const string& name, mat3x2 value) {
  UniformImpl(glProgramUniformMatrix3x2fv, name, 1, GL_FALSE, value.data());
}
void Program::Uniform(const string& name, mat3 value) {
  UniformImpl(glProgramUniformMatrix3fv, name, 1, GL_FALSE, value.data());
}
void Program::Uniform(const string& name, mat3x4 value) {
  UniformImpl(glProgramUniformMatrix3x4fv, name, 1, GL_FALSE, value.data());
}
void Program::Uniform(const string& name, mat4x2 value) {
  UniformImpl(glProgramUniformMatrix4x2fv, name, 1, GL_FALSE, value.data());
}
void Program::Uniform(const string& name, mat4x3 value) {
  UniformImpl(glProgramUniformMatrix4x3fv, name, 1, GL_FALSE, value.data());
}
void Program::Uniform(const string& name, mat4 value) {
  UniformImpl(glProgramUniformMatrix4fv, name, 1, GL_FALSE, value.data());
}
void Program::Uniform(const string& name, dmat2 value) {
  UniformImpl(glProgramUniformMatrix2dv, name, 1, GL_FALSE, value.data());
}
void Program::Uniform(const string& name, dmat2x3 value) {
  UniformImpl(glProgramUniformMatrix2x3dv, name, 1, GL_FALSE, value.data());
}
void Program::Uniform(const string& name, dmat2x4 value) {
  UniformImpl(glProgramUniformMatrix2x4dv, name, 1, GL_FALSE, value.data());
}
void Program::Uniform(const string& name, dmat3x2 value) {
  UniformImpl(glProgramUniformMatrix3x2dv, name, 1, GL_FALSE, value.data());
}
void Program::Uniform(const string& name, dmat3 value) {
  UniformImpl(glProgramUniformMatrix3dv, name, 1, GL_FALSE, value.data());
}
void Program::Uniform(const string& name, dmat3x4 value) {
  UniformImpl(glProgramUniformMatrix3x4dv, name, 1, GL_FALSE, value.data());
}
void Program::Uniform(const string& name, dmat4x2 value) {
  UniformImpl(glProgramUniformMatrix4x2dv, name, 1, GL_FALSE, value.data());
}
void Program::Uniform(const string& name, dmat4x3 value) {
  UniformImpl(glProgramUniformMatrix4x3dv, name, 1, GL_FALSE, value.data());
}
void Program::Uniform(const string& name, dmat4 value) {
  UniformImpl(glProgramUniformMatrix4dv, name, 1, GL_FALSE, value.data());
}

GLint Program::Get(GLenum pname) const {
  GLint params;
  glGetProgramiv(program_, pname, &params);
  ThrowIfError();
  return params;
}

}  // namespace gl
