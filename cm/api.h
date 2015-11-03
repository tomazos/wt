#pragma once

#include "cm/state.h"

namespace cm {

template <typename T>
uintptr_t GetTypeId() {
  static char x;
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
bool IsTypeId(void* userdata) {
  return *static_cast<uintptr_t*>(GetTypeIdStorage(userdata)) == GetTypeId<T>();
}

template <typename T>
void CheckTypeId(void* userdata) {
  if (!IsTypeId<T>(userdata)) throw std::logic_error("userdata type mismatch");
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
  if (address % alignof(T) != 0) throw std::logic_error("misaligned pointer");
}

template <typename T>
int DestroyObject(lua_State* L) {
  void* userdata = lua_touserdata(L, 1);
  if (!userdata) throw std::logic_error("destructor attached to non-userdata");
  CheckTypeId<T>(userdata);
  T* object = static_cast<T*>(GetObjectStorage<T>(userdata));
  object->~T();
  NullTypeId(userdata);
  return 0;
}

template <typename T>
void PushObjectMetatable(State* state) {
  int top = state->StackSize();
  state->PushString("object_metatables");
  state->PushField(State::REGISTRY);
  if (state->GetType(-1) == Type::NIL) {
    state->Pop();
    state->PushNewTable();
    state->PushString("object_metatables");
    state->PushCopy(-2);
    state->PopField(State::REGISTRY);
  }
  MUST(state->GetType(-1) == Type::TABLE);
  state->PushInteger(GetTypeId<T>());
  state->PushField(-2);
  if (state->GetType(-1) == Type::NIL) {
    state->Pop();
    state->PushNewTable();
    state->PushString("__gc");
    state->PushCFunction(DestroyObject<T>);
    state->PopField(-3);
    state->PushInteger(GetTypeId<T>());
    state->PushCopy(-2);
    state->PopField(-4);
  }
  MUST(state->GetType(-2) == Type::TABLE);
  MUST(state->GetType(-1) == Type::TABLE);
  state->Remove(-2);
  MUST(state->StackSize(), top + 1);
}

template <typename T, typename... Args>
void EmplaceObject(State* state, Args&&... args) {
  void* userdata = state->PushNewUserdata(ObjectStorageSize<T>());
  CheckAlignment<uintptr_t>(GetTypeIdStorage(userdata));
  CheckAlignment<T>(GetObjectStorage<T>(userdata));
  SetTypeId<T>(userdata);
  CheckTypeId<T>(userdata);
  T* object = static_cast<T*>(GetObjectStorage<T>(userdata));
  new (object) T(std::forward<Args>(args)...);
  PushObjectMetatable<T>(state);
  state->PopMetatable(-2);
}

template <typename T>
T& ToObject(State* state, State::Index index) {
  if (state->GetType(index) != Type::USERDATA) state->Throw("not a userdata");
  void* userdata = state->ToUserdata(index);
  CheckTypeId<T>(userdata);
  T* object = static_cast<T*>(GetObjectStorage<T>(userdata));
  return *object;
}

int DispatchFunction(lua_State* L);

inline void PushFunction(State* state, std::function<int()> f) {
  EmplaceObject<std::function<int()>>(state, std::move(f));
  state->PushCFunction(DispatchFunction, 1);
}

}  // namespace cm
