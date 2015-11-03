#pragma once

#include <map>

#include "experimental/sdl/attrib.h"
#include "experimental/sdl/vertex_array.h"
#include "gl/geometry.h"

namespace gl {

class AttribArrays {
 public:
  AttribArrays() = default;

  void Write(const Attrib& attrib, const std::vector<vec3>& data);
  void Write(const Attrib& attrib, const std::vector<vec4>& data);

  void Draw(GLenum mode);

 private:
  VertexArray vertex_array_;

  size_t size_;
  GLuint next_bindingindex_ = 1;
  std::map<string, GLuint> bindingindexes_;
  std::map<GLuint, optional<Buffer>> buffers_;
};

}  // namespace gl
