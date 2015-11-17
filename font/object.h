#pragma once

#include "font/freetype.h"
#include "font/error.h"

namespace font {

template <typename T, FT_Error (*deleter)(T)>
class Object {
 public:
  STATIC_ASSERT(std::is_pointer<T>::value);
  using Type = T;
  using Inner = typename std::remove_pointer<Type>::type;

  Object() : object_(nullptr) {}

  template <typename Creator, typename... Args>
  Object(Creator creator, Args&&... args)
      : object_(nullptr) {
    ThrowOnError(creator(std::forward<Args>(args)..., &object_));
  }

  explicit Object(Type object) : object_(object) {}

  Object(Object&& other) : object_(other.object_) { other.object_ = nullptr; }

  Object& operator=(Object&& other) {
    Clear();
    object_ = other.object_;
    other.object_ = nullptr;
  }

  ~Object() { Clear(); }

  operator Type() const { return object_; }

  Type operator->() const { return object_; }

  Type* ptr() { return &object_; }

 private:
  void Clear() {
    if (object_) {
      ThrowOnError(deleter(object_));
      object_ = nullptr;
    }
  }

  Type object_;

  Object(const Object&) = delete;
  Object& operator=(const Object&) = delete;
};

}  // namespace font
