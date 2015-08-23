#include "xxua/state.h"

#include <cstring>

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
  Allocator& allocator = *static_cast<Allocator*>(ud);
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
  void* const extra_space = lua_getextraspace(L);
  const void* const state_void = this;
  STATIC_ASSERT(sizeof(void*) == LUA_EXTRASPACE);
  std::memcpy(extra_space, &state_void, LUA_EXTRASPACE);
}

void State::Close() {
  if (L) {
    lua_close(L);
  }
}

State::CFunction State::AtPanic(CFunction panic_function) {
  return lua_atpanic(L, panic_function);
}

static std::string LastWriterError() {
  thread_local std::string s;
  return s;
}

static int WriterFunction(lua_State* L, const void* p, size_t sz, void* ud) {
  Writer& writer = *static_cast<Writer*>(ud);
  try {
    writer.Write(p, sz);
    return 0;
  } catch (std::exception e) {
    LastWriterError() = e.what();
    return 1;
  }
}

void State::Save(Writer& writer, bool strip) {
  void* ud = &writer;
  if (lua_dump(L, WriterFunction, ud, strip) == 0)
    return;
  else
    throw std::runtime_error(LastWriterError());
}

static const char* ReaderFunction(lua_State* L, void* ud, size_t* size) {
  Reader& reader = *static_cast<Reader*>(ud);
  Reader::Buffer buffer = reader.Read();
  *size = buffer.size;
  return buffer.data;
}

void State::Load(Reader& reader, const string& chunkname, const char* mode) {
  lua_load(L, ReaderFunction, &reader, chunkname.c_str(), mode);
}

State::~State() { Close(); }

}  // namespace xxua
