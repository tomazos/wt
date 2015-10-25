#include "experimental/sdl/object.h"

#include "experimental/sdl/error.h"

namespace gl {

Object::Object(Deleter object_deleter) : deleter_(object_deleter), name_(0) {}

Object::Object(GLuint name, Deleter deleter) : deleter_(deleter), name_(name) {
  if (name == 0) ThrowError();
}

Object::Object(Object&& original) : name_(original.name_) {
  original.name_ = 0;
}

Object& Object::operator=(Object&& original) {
  Delete();
  name_ = original.name_;
  original.name_ = 0;
  return *this;
}

Object::~Object() {
  Delete();
  name_ = 0;
}

Object::operator GLuint() const { return name_; }

void Object::Delete() {
  deleter_(name_);
  ThrowIfError();
  name_ = 0;
}

}  // namespace gl
