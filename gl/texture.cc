#include "gl/texture.h"

#include "core/must.h"
#include "gl/error.h"

namespace gl {

namespace {

auto create_texture = [] {
  GLuint name = 0;
  glGenTextures(1, &name);
  ThrowIfError();
  return name;
};

auto delete_texture = [](GLuint name) { glDeleteTextures(1, &name); };

}  // namespace

Texture::Texture() : texture_(create_texture(), delete_texture) {}

void Texture::Active(GLenum texture_unit) {
  glActiveTexture(texture_unit);
  ThrowIfError();
}

void Texture::Bind(GLenum target) {
  glBindTexture(target, texture_);
  ThrowIfError();
}

void Texture::Image2D(GLenum target, GLint level, GLint internalformat,
                      GLsizei width, GLsizei height, GLenum format, GLenum type,
                      const void *pixels) {
  glTexImage2D(target, level, internalformat, width, height, 0 /*border*/,
               format, type, pixels);
  ThrowIfError();
}

}  // namespace gl
