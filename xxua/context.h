#pragma once

#include "xxua/state.h"

namespace xxua {

class Context {
 public:
  inline Context(State& state);
  inline ~Context();

  inline static State*& Current();

 private:
  State* previous_;
};

using Integer = State::Integer;
using Unsigned = State::Unsigned;
using Float = State::Float;
using CFunction = State::CFunction;
using Index = State::Index;
using ChunkFormat = State::ChunkFormat;

inline Index UPVALUE(int i);

[[noreturn]] inline void Throw();
[[noreturn]] inline void Throw(string_view error_message);

// stack query
inline Index AbsIndex(Index idx);
inline int StackSize();
inline bool CheckStack(int n);

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
inline optional<Integer> FloatToInteger(Float n);
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
inline void MOD();
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

inline void Load(Reader& reader, const string& chunkname = "",
                 ChunkFormat format = ChunkFormat::EITHER);
inline void LoadFromStream(std::istream& is, const string& chunkname = "",
                           ChunkFormat format = ChunkFormat::EITHER);
inline void LoadFromString(string_view s, const string& chunkname = "",
                           ChunkFormat format = ChunkFormat::EITHER);

inline void Save(Writer& writer, bool strip = false);
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

inline State*& Context::Current() {
  thread_local State* current = nullptr;
  return current;
}

inline Context::Context(State& state) : previous_(Current()) {
  Current() = &state;
}

inline Context::~Context() { Current() = previous_; }

inline Index UPVALUE(int i) { return State::UPVALUE(i); }

[[noreturn]] inline void Throw() { Context::Current()->Throw(); }
[[noreturn]] inline void Throw(string_view error_message) {
  Context::Current()->Throw(error_message);
}

// stack query
inline Index AbsIndex(Index idx) { return Context::Current()->AbsIndex(idx); }
inline Index Top() { return Context::Current()->Top(); }
inline int StackSize() { return Context::Current()->StackSize(); }
inline bool CheckStack(int n) { return Context::Current()->CheckStack(n); }

// stack mutate
inline void ResizeStack(Index n) { return Context::Current()->ResizeStack(n); }
inline void Insert(Index index) { return Context::Current()->Insert(index); }
inline void Rotate(Index index, int offset) {
  return Context::Current()->Rotate(index, offset);
}
inline void Copy(Index from_index, Index to_index) {
  return Context::Current()->Copy(from_index, to_index);
}
inline void PushCopy(Index index) {
  return Context::Current()->PushCopy(index);
}
inline void Pop(int n) { Context::Current()->Pop(n); }
inline void Remove(Index index) { Context::Current()->Remove(index); }
inline void Replace(Index index) { Context::Current()->Replace(index); }

// element query
inline Type GetType(Index index) { return Context::Current()->GetType(index); }
inline bool ToBoolean(Index index) {
  return Context::Current()->ToBoolean(index);
}
inline Integer ToInteger(Index index) {
  return Context::Current()->ToInteger(index);
}
inline Float ToFloat(Index index) { return Context::Current()->ToFloat(index); }
inline optional<Integer> FloatToInteger(Float n) {
  return State::FloatToInteger(n);
}
inline string_view ToString(Index index) {
  return Context::Current()->ToString(index);
}
inline CFunction ToCFunction(Index index) {
  return Context::Current()->ToCFunction(index);
}
inline void* ToUserdata(Index index) {
  return Context::Current()->ToUserdata(index);
}
inline const void* ToPointer(Index index) {
  return Context::Current()->ToPointer(index);
}

// element creation
inline void PushNil() { Context::Current()->PushNil(); }
inline void PushBoolean(bool b) { Context::Current()->PushBoolean(b); }
inline void PushInteger(Integer i) { Context::Current()->PushInteger(i); }
inline void PushFloat(Float n) { Context::Current()->PushFloat(n); }
inline void PushString(string_view s) { Context::Current()->PushString(s); }
inline void PushLightUserdata(void* p) {
  Context::Current()->PushLightUserdata(p);
}
inline void PushNewTable(int narr, int nrec) {
  Context::Current()->PushNewTable(narr, nrec);
}
inline void* PushNewUserdata(size_t size) {
  return Context::Current()->PushNewUserdata(size);
}
inline void PushCFunction(CFunction function, int num_captures) {
  Context::Current()->PushCFunction(function, num_captures);
}

// arithmetic and comparison
inline void ADD() { Context::Current()->ADD(); }
inline void SUB() { Context::Current()->SUB(); }
inline void MUL() { Context::Current()->MUL(); }
inline void DIV() { Context::Current()->DIV(); }
inline void MOD() { Context::Current()->MOD(); }
inline void NEG() { Context::Current()->NEG(); }
inline void BNOT() { Context::Current()->BNOT(); }
inline void BAND() { Context::Current()->BAND(); }
inline void BOR() { Context::Current()->BOR(); }
inline void XOR() { Context::Current()->XOR(); }
inline void SHL() { Context::Current()->SHL(); }
inline void SHR() { Context::Current()->SHR(); }
inline bool EQ(Index index1, Index index2, bool raw) {
  return Context::Current()->EQ(index1, index2, raw);
}
inline bool NE(Index index1, Index index2, bool raw) {
  return Context::Current()->NE(index1, index2, raw);
}
inline bool LT(Index index1, Index index2) {
  return Context::Current()->LT(index1, index2);
}
inline bool GT(Index index1, Index index2) {
  return Context::Current()->GT(index1, index2);
}
inline bool LE(Index index1, Index index2) {
  return Context::Current()->LE(index1, index2);
}
inline bool GE(Index index1, Index index2) {
  return Context::Current()->GE(index1, index2);
}
inline void LEN(Index index, bool raw) { Context::Current()->LEN(index, raw); }
inline void Concat(int n) { Context::Current()->Concat(n); }

// function calls
inline void Call(int nargs, int nresults) {
  Context::Current()->Call(nargs, nresults);
}
inline void CallMultret(int nargs) { Context::Current()->CallMultret(nargs); }
inline void CallProtected(int nargs, int nresults, Index msgh) {
  Context::Current()->CallProtected(nargs, nresults, msgh);
}

// chunk loading/saving

inline void Load(Reader& reader, const string& chunkname, ChunkFormat format) {
  Context::Current()->Load(reader, chunkname, format);
}
inline void LoadFromStream(std::istream& is, const string& chunkname,
                           ChunkFormat format) {
  Context::Current()->LoadFromStream(is, chunkname, format);
}
inline void LoadFromString(string_view s, const string& chunkname,
                           ChunkFormat format) {
  Context::Current()->LoadFromString(s, chunkname, format);
}

inline void Save(Writer& writer, bool strip) {
  Context::Current()->Save(writer, strip);
}
inline void SaveToStream(std::ostream& os, bool strip) {
  Context::Current()->SaveToStream(os, strip);
}
inline string SaveToString(bool strip) {
  return Context::Current()->SaveToString(strip);
}

// garbage collector
inline void GCStop() { Context::Current()->GCStop(); }
inline void GCRestart() { Context::Current()->GCRestart(); }
inline void GCCollect() { Context::Current()->GCCollect(); }
inline int64 GCCount() { return Context::Current()->GCCount(); }
inline void GCStep() { Context::Current()->GCStep(); }
inline bool GCSetPause(bool pause) {
  return Context::Current()->GCSetPause(pause);
}
inline void GCSetStepMultiplier(int step_multiplier) {
  Context::Current()->GCSetStepMultiplier(step_multiplier);
}
inline bool GCIsRunning() { return Context::Current()->GCIsRunning(); }

// global table
inline void PushGlobalTable() { Context::Current()->PushGlobalTable(); }

// full userdata
inline void PopUservalue(Index userdata_index) {
  Context::Current()->PopUservalue(userdata_index);
}
inline void PushUservalue(Index userdata_index) {
  Context::Current()->PushUservalue(userdata_index);
}

// tables
inline void PopField(Index table, bool raw) {
  Context::Current()->PopField(table, raw);
}
inline void PushField(Index table, bool raw) {
  Context::Current()->PushField(table, raw);
}
[[gnu::warn_unused_result]] inline bool Next(Index table) {
  return Context::Current()->Next(table);
}

// metatable
inline void PopMetatable(Index index) {
  Context::Current()->PopMetatable(index);
}
[[gnu::warn_unused_result]] inline bool PushMetatable(Index index) {
  return Context::Current()->PushMetatable(index);
}

}  //  namespace xxua
