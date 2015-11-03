#pragma once

#include <vector>

#include "experimental/sdl/object.h"
#include "experimental/sdl/opengl.h"

namespace gl {

class Buffer {
 public:
  Buffer();

  // GL_ARRAY_BUFFER
  // GL_ATOMIC_COUNTER_BUFFER
  // GL_COPY_READ_BUFFER
  // GL_COPY_WRITE_BUFFER
  // GL_DISPATCH_INDIRECT_BUFFER
  // GL_DRAW_INDIRECT_BUFFER
  // GL_ELEMENT_ARRAY_BUFFER
  // GL_PIXEL_PACK_BUFFER
  // GL_PIXEL_UNPACK_BUFFER
  // GL_QUERY_BUFFER
  // GL_SHADER_STORAGE_BUFFER
  // GL_TEXTURE_BUFFER
  // GL_TRANSFORM_FEEDBACK_BUFFER
  // GL_UNIFORM_BUFFER
  Buffer(const void* data, size_t size, GLenum target, GLenum usage);

  template <typename T>
  Buffer(const std::vector<T>& data, GLenum target, GLenum usage)
      : Buffer((const void*)data.data(), data.size() * sizeof(T), target,
               usage) {}

 private:
  void Bind(GLenum target);
  Object buffer_;

  friend class VertexArray;
};

}  // namespace gl
