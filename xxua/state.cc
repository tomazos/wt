#include "xxua/state.h"

namespace xxua {

static void* DefaultAllocate(void* ud[[gnu::unused]], void* ptr,
                             size_t osize[[gnu::unused]], size_t nsize) {
  if (nsize == 0) {
    free(ptr);
    return NULL;
  } else
    return realloc(ptr, nsize);
}

static Allocator::BlockPurpose TranslateBlockPurpose(size_t osize) {
  switch (osize) {
    case LUA_TSTRING:
      return Allocator::STRING;
    case LUA_TTABLE:
      return Allocator::TABLE;
    case LUA_TFUNCTION:
      return Allocator::FUNCTION;
    case LUA_TUSERDATA:
      return Allocator::USERDATA;
    case LUA_TTHREAD:
      return Allocator::THREAD;
    default:
      return Allocator::OTHER;
  }
}

static void* CustomAllocate(void* ud, void* ptr, size_t osize, size_t nsize) {
  Allocator& allocator = *reinterpret_cast<Allocator*>(ud);
  if (ptr == nullptr) {
    if (nsize == 0) return nullptr;
    const auto block_purpose = TranslateBlockPurpose(osize);
    return allocator.Create(block_purpose, nsize);
  } else {
    if (nsize != 0) {
      return allocator.Resize(ptr, osize, nsize);
    } else {
      allocator.Destroy(ptr, osize);
      return nullptr;
    }
  }
}

[[noreturn]] static int PanicThrow(lua_State* L) {
  throw std::runtime_error(lua_tostring(L, -1));
}

State::State() : State(lua_newstate(DefaultAllocate, nullptr)) {}

State::State(Allocator& allocator)
    : State(lua_newstate(CustomAllocate, &allocator)) {}

State::State(lua_State* L_in) : L(L_in) {
  if (L == nullptr)
    throw std::runtime_error("Could not create a new lua state.");
  AtPanic(PanicThrow);
}

State::State(State&& other) : L(other.L) { other.L = nullptr; }

void State::Close() {
  if (L) {
    lua_close(L);
  }
}

void State::AtPanic(lua_CFunction panic_function) {
  lua_atpanic(L, panic_function);
}

State::~State() { Close(); }

State& State::operator=(State&& other) {
  Close();
  L = other.L;
  other.L = nullptr;
  return *this;
}

}  // namespace xxua
