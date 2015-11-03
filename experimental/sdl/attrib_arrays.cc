#include "experimental/sdl/attrib_arrays.h"

#include "core/must.h"

namespace gl {

void AttribArrays::Write(const Attrib& attrib, const std::vector<vec3>& data) {
  if (bindingindexes_.empty()) {
    size_ = data.size();
  } else {
    MUST_EQ(data.size(), size_);
  }

  GLuint& bindingindex = bindingindexes_[attrib.name()];

  if (bindingindex == 0) bindingindex = next_bindingindex_++;

  vertex_array_.EnableAttribArray(attrib);
  vertex_array_.BindAttrib(attrib, bindingindex);
  vertex_array_.AttribFormat(attrib, 3, GL_FLOAT, GL_FALSE,
                             0 /*relativeoffset*/);

  buffers_[bindingindex].emplace(data, GL_ARRAY_BUFFER, GL_STATIC_DRAW);

  vertex_array_.BindBuffer(bindingindex, *buffers_[bindingindex], 0 /*offset*/,
                           sizeof(vec3));
}

void AttribArrays::Write(const Attrib& attrib, const std::vector<vec4>& data) {
  if (bindingindexes_.empty()) {
    size_ = data.size();
  } else {
    MUST_EQ(data.size(), size_);
  }

  GLuint& bindingindex = bindingindexes_[attrib.name()];

  if (bindingindex == 0) bindingindex = next_bindingindex_++;

  vertex_array_.EnableAttribArray(attrib);
  vertex_array_.BindAttrib(attrib, bindingindex);
  vertex_array_.AttribFormat(attrib, 4, GL_FLOAT, GL_FALSE,
                             0 /*relativeoffset*/);

  buffers_[bindingindex].emplace(data, GL_ARRAY_BUFFER, GL_STATIC_DRAW);

  vertex_array_.BindBuffer(bindingindex, *buffers_[bindingindex], 0 /*offset*/,
                           sizeof(vec4));
}

void AttribArrays::Draw(GLenum mode) {
  MUST(!bindingindexes_.empty());
  vertex_array_.Draw(mode, 0 /*first*/, size_);
}

}  // namespace gl
