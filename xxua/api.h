#pragma once

#include "xxua/context.h"
#include "xxua/pointer.h"
#include "xxua/reference.h"
#include "xxua/state.h"

namespace xxua {

template <typename T>
uintptr_t GetTypeId() {
  static int x;
  void* p = &x;
  return reinterpret_cast<uintptr_t>(p);
}

inline void* GetTypeIdStorage(void* userdata) { return userdata; }

template <typename T>
void SetTypeId(void* userdata) {
  *static_cast<uintptr_t*>(GetTypeIdStorage(userdata)) = GetTypeId<T>();
}

inline void NullTypeId(void* userdata) {
  *static_cast<uintptr_t*>(GetTypeIdStorage(userdata)) = 0;
}

template <typename T>
void CheckTypeId(void* userdata) {
  if (*static_cast<uintptr_t*>(GetTypeIdStorage(userdata)) != GetTypeId<T>())
    Throw("userdata type mismatch");
}

template <typename T>
uintptr_t GetObjectOffset() {
  uintptr_t offset = sizeof(uintptr_t);
  if (offset % alignof(T) == 0) return offset;
  offset += alignof(T) - (offset % alignof(T));
  if (offset % alignof(T) != 0)
    throw std::logic_error("GetObjectOffset logic error");
  return offset;
}

template <typename T>
constexpr uintptr_t ObjectStorageSize() {
  return GetObjectOffset<T>() + sizeof(T);
}

template <typename T>
void* GetObjectStorage(void* userdata) {
  char* p = static_cast<char*>(userdata);
  p += GetObjectOffset<T>();
  return static_cast<void*>(p);
}

template <typename T>
void CheckAlignment(void* ptr) {
  uintptr_t address = reinterpret_cast<uintptr_t>(ptr);
  if (address % alignof(T) != 0) Throw("misaligned pointer");
}

template <typename T>
int DestroyObject(lua_State* L) {
  void* userdata = lua_touserdata(L, 1);
  if (!userdata) Throw("DestroyObject expected userdata");
  CheckTypeId<T>(userdata);
  T* object = static_cast<T*>(GetObjectStorage<T>(userdata));
  object->~T();
  NullTypeId(userdata);
  return 0;
}

template <typename T, typename... Args>
void EmplaceObject(Args&&... args) {
  void* userdata = PushNewUserdata(ObjectStorageSize<T>());
  CheckAlignment<uintptr_t>(GetTypeIdStorage(userdata));
  CheckAlignment<T>(GetObjectStorage<T>(userdata));
  SetTypeId<T>(userdata);
  CheckTypeId<T>(userdata);
  T* object = static_cast<T*>(GetObjectStorage<T>(userdata));
  new (object) T(std::forward<Args>(args)...);
  PushNewTable();
  PushString("__gc");
  PushCFunction(DestroyObject<T>);
  PopField(-3);
  PopMetatable(-2);
}

template <typename T>
T& ToObject(Index index) {
  if (GetType(index) != Type::USERDATA) Throw("not a userdata");
  void* userdata = ToUserdata(index);
  CheckTypeId<T>(userdata);
  T* object = static_cast<T*>(GetObjectStorage<T>(userdata));
  return *object;
}

int DispatchFunction(lua_State* L);

inline void PushFunction(std::function<int()> f) {
  EmplaceObject<std::function<int()>>(std::move(f));
  PushCFunction(DispatchFunction, 1);
}

}  // namespace xxua
