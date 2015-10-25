#include "experimental/sdl/vertex_array.h"

#include "core/must.h"
#include "experimental/sdl/error.h"

namespace gl {

namespace {

auto create_vertex_array = [] {
  GLuint name = 0;
  glGenVertexArrays(1, &name);
  ThrowIfError();
  return name;
};

auto delete_vertex_array = [](GLuint name) { glDeleteVertexArrays(1, &name); };

}  // namespace

VertexArray::VertexArray()
    : array_(create_vertex_array(), delete_vertex_array) {}

void VertexArray::EnableAttribArray(const Attrib& attrib) {
  Bind();
  glEnableVertexAttribArray(attrib.location_);
  ThrowIfError();
}

void VertexArray::DisableAttribArray(const Attrib& attrib) {
  Bind();
  glDisableVertexAttribArray(attrib.location_);
  ThrowIfError();
}

void VertexArray::AttribFormat(Attrib& attrib, GLint size, GLenum type,
                               GLboolean normalized, GLuint relativeoffset) {
  Bind();
  glVertexAttribFormat(attrib.location_, size, type, normalized,
                       relativeoffset);
  ThrowIfError();
}

void VertexArray::AttribIFormat(Attrib& attrib, GLint size, GLenum type,
                                GLuint relativeoffset) {
  Bind();
  glVertexAttribIFormat(attrib.location_, size, type, relativeoffset);
  ThrowIfError();
}

void VertexArray::AttribLFormat(Attrib& attrib, GLint size, GLenum type,
                                GLuint relativeoffset) {
  Bind();
  glVertexAttribLFormat(attrib.location_, size, type, relativeoffset);
  ThrowIfError();
}

void VertexArray::BindAttrib(const Attrib& attrib, GLuint bindingindex) {
  Bind();
  glVertexAttribBinding(attrib.location_, bindingindex);
  ThrowIfError();
}

void VertexArray::BindBuffer(GLuint bindingindex, const Buffer& buffer,
                             GLintptr offset, GLintptr stride) {
  Bind();
  glBindVertexBuffer(bindingindex, buffer.buffer_, offset, stride);
  ThrowIfError();
}

void VertexArray::Bind() {
  glBindVertexArray(array_);
  ThrowIfError();
}

void VertexArray::Draw(GLenum mode, GLint first, GLsizei count) {
  Bind();
  glDrawArrays(mode, first, count);
  ThrowIfError();
}

}  // namespace gl
