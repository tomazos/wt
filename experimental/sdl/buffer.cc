#include "experimental/sdl/buffer.h"

#include "core/must.h"
#include "experimental/sdl/error.h"

namespace gl {

namespace {

auto create_buffer = [] {
  GLuint name = 0;
  glGenBuffers(1, &name);
  ThrowIfError();
  return name;
};

auto delete_buffer = [](GLuint name) { glDeleteBuffers(1, &name); };

}  // namespace

Buffer::Buffer() : buffer_(create_buffer(), delete_buffer) {}

Buffer::Buffer(const void* data, size_t size, GLenum target, GLenum usage)
    : Buffer() {
  Bind(target);
  glBufferData(target, size, data, usage);
  ThrowIfError();
}

void Buffer::Bind(GLenum target) {
  glBindBuffer(target, buffer_);
  ThrowIfError();
}

}  // namespace gl
