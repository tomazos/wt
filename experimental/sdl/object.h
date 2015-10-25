#pragma once

#include "experimental/sdl/opengl.h"

namespace gl {

class Object {
 public:
  using Deleter = void (*)(GLuint name);

  Object(Deleter deleter);
  explicit Object(GLuint name, Deleter deleter);
  Object(Object&& original);

  Object& operator=(Object&& original);
  ~Object();
  operator GLuint() const;

 private:
  void Delete();

  Deleter deleter_;
  GLuint name_;

  Object(const Object&) = delete;
  Object& operator=(const Object&) = delete;
};

}  // namespace gl
