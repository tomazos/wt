#pragma once

#include "lua/lua.h"
#include "xxua/allocator.h"
#include "xxua/reader.h"
#include "xxua/type.h"
#include "xxua/writer.h"

namespace xxua {

class State {
 public:
  using Integer = lua_Integer;
  using Unsigned = lua_Unsigned;
  using Float = lua_Number;
  using CFunction = lua_CFunction;
  using Index = int;

  State();
  explicit State(Allocator& allocator);
  ~State();

  [[noreturn]] inline void Error();
  [[noreturn]] inline void Error(const string& error_message);

  // stack query
  inline Index AbsIndex(Index idx);
  inline Index Top();
  inline int StackSize();
  inline bool CheckStack(int n);

  // pseudo-indexes
  static constexpr Index REGISTRY = LUA_REGISTRYINDEX;
  inline static Index UPVALUE(int i);

  // stack mutate
  inline void ResizeStack(Index n);
  inline void Insert(Index index);
  inline void Rotate(Index index, int offset);
  inline void Copy(Index from_index, Index to_index);
  inline void PushCopy(Index index);
  inline void Pop(int n = 1);
  inline void Remove(Index index);
  inline void Replace(Index index);

  // element query
  inline Type GetType(Index index);
  inline bool ToBoolean(Index index);
  inline Integer ToInteger(Index index);
  inline Float ToFloat(Index index);
  inline static optional<Integer> FloatToInteger(Float n);
  inline string_view ToString(Index index);
  inline CFunction ToCFunction(Index index);
  inline void* ToUserdata(Index index);
  inline const void* ToPointer(Index index);

  // element creation
  inline void PushNil();
  inline void PushBoolean(bool b);
  inline void PushInteger(Integer i);
  inline void PushFloat(Float n);
  inline void PushString(string_view s);
  inline void PushLightUserdata(void* p);
  inline void PushNewTable(int narr = 0, int nrec = 0);
  inline void* PushNewUserdata(size_t size);
  inline void PushCFunction(CFunction function, int num_captures = 0);

  // arithmetic and comparison
  inline void ADD();
  inline void SUB();
  inline void MUL();
  inline void DIV();
  inline void IDIV();
  inline void MOD();
  inline void POW();
  inline void NEG();
  inline void BNOT();
  inline void BAND();
  inline void BOR();
  inline void XOR();
  inline void SHL();
  inline void SHR();
  inline bool EQ(Index index1, Index index2, bool raw = false);
  inline bool NE(Index index1, Index index2, bool raw = false);
  inline bool LT(Index index1, Index index2);
  inline bool GT(Index index1, Index index2);
  inline bool LE(Index index1, Index index2);
  inline bool GE(Index index1, Index index2);
  inline void LEN(Index index, bool raw = false);
  inline void Concat(int n);

  // function calls
  inline void Call(int nargs, int nresults);
  inline void CallMultret(int nargs);
  inline void CallProtected(int nargs, int nresults, Index msgh = 0);

  // chunk loading/saving
  enum class ChunkFormat { EITHER, BINARY, TEXT };

  inline void Load(Reader& reader, const string& chunkname = "",
                   ChunkFormat format = ChunkFormat::EITHER);
  inline void LoadFromStream(std::istream& is, const string& chunkname = "",
                             ChunkFormat format = ChunkFormat::EITHER);
  inline void LoadFromString(const string& s, const string& chunkname = "",
                             ChunkFormat format = ChunkFormat::EITHER);

  void Save(Writer& writer, bool strip = false);
  inline void SaveToStream(std::ostream& os, bool strip = false);
  inline string SaveToString(bool strip = false);

  // garbage collector
  inline void GCStop();
  inline void GCRestart();
  inline void GCCollect();
  inline int64 GCCount();
  inline void GCStep();
  inline bool GCSetPause(bool pause);
  inline void GCSetStepMultiplier(int step_multiplier);
  inline bool GCIsRunning();

  // global table
  inline void PushGlobalTable();

  // full userdata
  inline void PopUservalue(Index userdata_index);
  inline void PushUservalue(Index userdata_index);

  // tables
  inline void PopField(Index table, bool raw = false);
  inline void PushField(Index table, bool raw = false);
  [[gnu::warn_unused_result]] inline bool Next(Index table);

  // metatable
  inline void PopMetatable(Index index);
  [[gnu::warn_unused_result]] inline bool PushMetatable(Index index);

 private:
  CFunction AtPanic(CFunction panic_function);
  void Load(Reader& reader, const string& chunkname, const char* mode);

  void Close();

  State(lua_State* L_in);

  lua_State* L;

  State(const State& state) = delete;
  State(State&& other) = delete;
  State& operator=(const State& other) = delete;
  State& operator=(State&& other) = delete;
};

[[noreturn]] inline void State::Error() { lua_error(L); }

[[noreturn]] inline void State::Error(const string& error_message) {
  PushString(error_message);
  Error();
}

inline State::Index State::AbsIndex(Index idx) { return lua_absindex(L, idx); }

inline void State::ADD() { lua_arith(L, LUA_OPADD); }
inline void State::SUB() { lua_arith(L, LUA_OPSUB); }
inline void State::MUL() { lua_arith(L, LUA_OPMUL); }
inline void State::DIV() { lua_arith(L, LUA_OPDIV); }
inline void State::IDIV() { lua_arith(L, LUA_OPIDIV); }
inline void State::MOD() { lua_arith(L, LUA_OPMOD); }
inline void State::POW() { lua_arith(L, LUA_OPPOW); }
inline void State::NEG() { lua_arith(L, LUA_OPUNM); }
inline void State::BNOT() { lua_arith(L, LUA_OPBNOT); }
inline void State::BAND() { lua_arith(L, LUA_OPBAND); }
inline void State::BOR() { lua_arith(L, LUA_OPBOR); }
inline void State::XOR() { lua_arith(L, LUA_OPBXOR); }
inline void State::SHL() { lua_arith(L, LUA_OPSHL); }
inline void State::SHR() { lua_arith(L, LUA_OPSHR); }
inline bool State::EQ(Index index1, Index index2, bool raw) {
  if (raw)
    return lua_rawequal(L, index1, index2);
  else
    return lua_compare(L, index1, index2, LUA_OPEQ);
}

inline bool State::NE(Index index1, Index index2, bool raw) {
  return !EQ(index1, index2, raw);
}

inline bool State::LT(Index index1, Index index2) {
  return lua_compare(L, index1, index2, LUA_OPLT);
}

inline bool State::GT(Index index1, Index index2) {
  return lua_compare(L, index2, index1, LUA_OPLT);
}

inline bool State::LE(Index index1, Index index2) {
  return lua_compare(L, index1, index2, LUA_OPLE);
}

inline bool State::GE(Index index1, Index index2) {
  return lua_compare(L, index2, index1, LUA_OPLE);
}

inline void State::LEN(Index index, bool raw) {
  if (raw)
    lua_rawlen(L, index);
  else
    lua_len(L, index);
}

inline void State::Call(int nargs, int nresults) {
  lua_call(L, nargs, nresults);
}

inline void State::CallMultret(int nargs) { lua_call(L, nargs, LUA_MULTRET); }

inline void State::CallProtected(int nargs, int nresults, Index msgh) {
  if (lua_pcall(L, nargs, nresults, msgh) == LUA_OK)
    return;
  else {
    std::string error_message = ToString(-1).to_string();
    Pop();
    throw std::runtime_error(error_message);
  }
}

inline bool State::CheckStack(int n) { return lua_checkstack(L, n); }

inline void State::Concat(int n) { lua_concat(L, n); }

inline void State::Copy(Index from_index, Index to_index) {
  lua_copy(L, from_index, to_index);
}

inline void State::SaveToStream(std::ostream& os, bool strip) {
  OutputStreamWriter writer(os);
  Save(writer, strip);
}

inline string State::SaveToString(bool strip) {
  std::ostringstream oss;
  SaveToStream(oss, strip);
  return oss.str();
}

inline void State::GCStop() { lua_gc(L, LUA_GCSTOP, 0); }

inline void State::GCRestart() { lua_gc(L, LUA_GCRESTART, 0); }

inline void State::GCCollect() { lua_gc(L, LUA_GCCOLLECT, 0); }

inline int64 State::GCCount() {
  int64 count = lua_gc(L, LUA_GCCOUNT, 0);
  count <<= 10;
  count += lua_gc(L, LUA_GCCOUNTB, 0);
  return count;
}

inline void State::GCStep() { lua_gc(L, LUA_GCSTEP, 0); }

inline bool State::GCSetPause(bool pause) {
  return lua_gc(L, LUA_GCSETPAUSE, pause);
}

inline void State::GCSetStepMultiplier(int step_multiplier) {
  lua_gc(L, LUA_GCSETSTEPMUL, step_multiplier);
}

inline bool State::GCIsRunning() { return lua_gc(L, LUA_GCISRUNNING, 0); }

inline void State::PushNil() { lua_pushnil(L); }
inline void State::PushBoolean(bool b) { lua_pushboolean(L, b); }
inline void State::PushInteger(Integer i) { lua_pushinteger(L, i); }
inline void State::PushFloat(Float n) { lua_pushnumber(L, n); }
inline void State::PushLightUserdata(void* p) { lua_pushlightuserdata(L, p); }
inline void State::PushString(string_view s) {
  lua_pushlstring(L, s.data(), s.size());
}
inline void State::PushNewTable(int narr, int nrec) {
  lua_createtable(L, narr, nrec);
}
inline void* State::PushNewUserdata(size_t size) {
  return lua_newuserdata(L, size);
}
inline void State::PushCFunction(CFunction function, int num_captures) {
  lua_pushcclosure(L, function, num_captures);
}

inline void State::PushUservalue(Index index) { lua_getuservalue(L, index); }

inline void State::PushGlobalTable() { lua_pushglobaltable(L); }
inline void State::PushCopy(Index index) { lua_pushvalue(L, index); }

inline void State::PopUservalue(Index index) { lua_setuservalue(L, index); }

inline void State::PushField(Index index, bool raw) {
  if (raw) {
    lua_rawget(L, index);
  } else {
    lua_gettable(L, index);
  }
}

[[gnu::warn_unused_result]] inline bool State::PushMetatable(Index index) {
  return lua_getmetatable(L, index);
}

inline void State::PopMetatable(Index index) { lua_setmetatable(L, index); }

inline State::Index State::Top() { return lua_gettop(L); }
inline int State::StackSize() { return Top(); }

inline void State::Insert(Index index) { lua_insert(L, index); }

inline Type State::GetType(Index index) {
  uint8 type = xlua_type(L, index);
  if ((type & 0b0'0000'0111) == LUA_TSTRING)
    return Type::STRING;
  else
    return static_cast<Type>(type);
}

inline void State::PopField(Index index, bool raw) {
  if (raw)
    lua_rawset(L, index);
  else
    lua_settable(L, index);
}

inline void State::Remove(Index index) { lua_remove(L, index); }

inline void State::Replace(Index index) { lua_replace(L, index); }

inline void State::Rotate(Index index, int offset) {
  lua_rotate(L, index, offset);
}

inline void State::Load(Reader& reader, const string& chunkname,
                        ChunkFormat format) {
  const char* format_string = nullptr;
  switch (format) {
    case ChunkFormat::EITHER:
      format_string = "bt";
      break;
    case ChunkFormat::BINARY:
      format_string = "b";
      break;
    case ChunkFormat::TEXT:
      format_string = "t";
      break;
  }
  Load(reader, chunkname, format_string);
}

inline void State::LoadFromStream(std::istream& is, const string& chunkname,
                                  ChunkFormat format) {
  InputStreamReader<> reader(is);
  Load(reader, chunkname, format);
}

inline void State::LoadFromString(const string& s, const string& chunkname,
                                  ChunkFormat format) {
  std::istringstream iss(s);
  LoadFromStream(iss, chunkname, format);
}

[[gnu::warn_unused_result]] inline bool State::Next(Index index) {
  return lua_next(L, index);
}

inline optional<State::Integer> State::FloatToInteger(Float n) {
  Integer i;
  if (lua_numbertointeger(n, &i))
    return i;
  else
    return nullopt;
}

inline void State::Pop(int n) { lua_pop(L, n); }

inline void State::ResizeStack(Index n) { lua_settop(L, n); }

inline bool State::ToBoolean(Index index) {
  if (GetType(index) != Type::BOOLEAN) Error("boolean expected");

  return lua_toboolean(L, index);
}

inline State::Integer State::ToInteger(Index index) {
  if (GetType(index) != Type::INTEGER) Error("integer expected");

  return lua_tointeger(L, index);
}

inline State::Float State::ToFloat(Index index) {
  if (GetType(index) != Type::FLOAT) Error("float expected");
  return lua_tonumber(L, index);
}

inline State::CFunction State::ToCFunction(Index index) {
  if (GetType(index) != Type::CFUNCTION) Error("C function expected");

  return lua_tocfunction(L, index);
}

inline string_view State::ToString(Index index) {
  if (GetType(index) != Type::STRING) Error("string expected");

  size_t len;
  const char* data = lua_tolstring(L, index, &len);
  return {data, len};
}

inline const void* State::ToPointer(Index index) {
  return lua_topointer(L, index);
}

inline void* State::ToUserdata(Index index) { return lua_touserdata(L, index); }

inline State::Index State::UPVALUE(int i) { return lua_upvalueindex(i); }

}  // namespace xxua
