#pragma once

#include "experimental/sdl/attrib.h"
#include "experimental/sdl/error.h"
#include "experimental/sdl/opengl.h"
#include "experimental/sdl/object.h"
#include "experimental/sdl/shader.h"
#include "experimental/sdl/types.h"

namespace gl {

class Program {
 public:
  Program();

  string GetInfoLog() const;

  void Attach(const Shader& shader);
  void Detach(const Shader& shader);

  Attrib BindAttrib(const string& name);

  void Link();
  void Use();

  void Uniform(const string& name, bool value);
  void Uniform(const string& name, bvec2 value);
  void Uniform(const string& name, bvec3 value);
  void Uniform(const string& name, bvec4 value);

  void Uniform(const string& name, int32 value);
  void Uniform(const string& name, ivec2 value);
  void Uniform(const string& name, ivec3 value);
  void Uniform(const string& name, ivec4 value);

  void Uniform(const string& name, uint32 value);
  void Uniform(const string& name, uvec2 value);
  void Uniform(const string& name, uvec3 value);
  void Uniform(const string& name, uvec4 value);

  void Uniform(const string& name, float32 value);
  void Uniform(const string& name, vec2 value);
  void Uniform(const string& name, vec3 value);
  void Uniform(const string& name, vec4 value);

  void Uniform(const string& name, float64 value);
  void Uniform(const string& name, dvec2 value);
  void Uniform(const string& name, dvec3 value);
  void Uniform(const string& name, dvec4 value);

  void Uniform(const string& name, mat2 value);
  void Uniform(const string& name, mat2x3 value);
  void Uniform(const string& name, mat2x4 value);
  void Uniform(const string& name, mat3x2 value);
  void Uniform(const string& name, mat3 value);
  void Uniform(const string& name, mat3x4 value);
  void Uniform(const string& name, mat4x2 value);
  void Uniform(const string& name, mat4x3 value);
  void Uniform(const string& name, mat4 value);
  void Uniform(const string& name, dmat2 value);
  void Uniform(const string& name, dmat2x3 value);
  void Uniform(const string& name, dmat2x4 value);
  void Uniform(const string& name, dmat3x2 value);
  void Uniform(const string& name, dmat3 value);
  void Uniform(const string& name, dmat3x4 value);
  void Uniform(const string& name, dmat4x2 value);
  void Uniform(const string& name, dmat4x3 value);
  void Uniform(const string& name, dmat4 value);

 private:
  GLuint BindAttribLocation(const string& name);

  void BindAttribLocation(GLuint index, const string& name);

  GLint GetAttribLocation(const string& name) const;
  GLint GetUniformLocation(const string& name) const;

  template <typename F, typename... Args>
  void UniformImpl(F f, const string& name, Args&&... args);

  GLint Get(GLenum pname) const;

  GLuint next_available_index = 1;
  Object program_;
};

}  // namespace gl
