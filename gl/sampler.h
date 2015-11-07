#pragma once

#include "gl/object.h"
#include "gl/opengl.h"

namespace gl {

class Sampler {
 public:
  Sampler();

  void Bind(GLuint unit);

 private:
  Object sampler_;
};

}  // namespace gl
