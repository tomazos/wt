#pragma once

#include "state.h"

namespace xxua {

class Reference {
 public:
  inline Reference& operator=(Reference& that);  // Copy
  inline void PushCopy();

  // element query
  inline Type GetType();
  inline bool ToBoolean();
  inline Integer ToInteger();
  inline Float ToFloat();
  inline string_view ToString();
  inline CFunction ToCFunction();
  inline void* ToUserdata();
  inline const void* ToPointer();

  inline void PushLength(bool raw = false);

  // full userdata
  inline void PopUservalue();
  inline void PushUservalue();

  // tables
  inline void PopField(bool raw = false);
  inline void PushField(bool raw = false);
  [[gnu::warn_unused_result]] inline bool Next();

  // metatable
  inline void PopMetatable();
  [[gnu::warn_unused_result]] inline bool PushMetatable();

 private:
  Reference(State* state, Index index)
      : state_(state), index_(state->AbsIndex(index)) {}

  State* const state_;
  const Index index_;

  friend class Pointer;
  friend class Stack;
  friend inline bool operator==(Reference a, Reference b);
  friend inline bool operator!=(Reference a, Reference b);
  friend inline bool operator<(Reference a, Reference b);
  friend inline bool operator>(Reference a, Reference b);
  friend inline bool operator<=(Reference a, Reference b);
  friend inline bool operator>=(Reference a, Reference b);
  friend inline Reference Registry(State& state);
};

inline bool operator==(Reference a, Reference b);
inline bool operator!=(Reference a, Reference b);
inline bool operator<(Reference a, Reference b);
inline bool operator>(Reference a, Reference b);
inline bool operator<=(Reference a, Reference b);
inline bool operator>=(Reference a, Reference b);

inline Reference& Reference::operator=(Reference& that) {
  state_->Copy(that.index_, index_);
  return *this;
}

inline void Reference::PushCopy() { state_->PushCopy(index_); }

inline bool operator==(Reference a, Reference b) {
  return a.state_->EQ(a.index_, b.index_);
}

inline bool operator!=(Reference a, Reference b) {
  return a.state_->NE(a.index_, b.index_);
}

inline bool operator<(Reference a, Reference b) {
  return a.state_->LE(a.index_, b.index_);
}

inline bool operator>(Reference a, Reference b) {
  return a.state_->GT(a.index_, b.index_);
}

inline bool operator<=(Reference a, Reference b) {
  return a.state_->LE(a.index_, b.index_);
}

inline bool operator>=(Reference a, Reference b) {
  return a.state_->GE(a.index_, b.index_);
}

inline Type Reference::GetType() { return state_->GetType(index_); }

inline bool Reference::ToBoolean() { return state_->ToBoolean(index_); }

inline Integer Reference::ToInteger() { return state_->ToInteger(index_); }

inline Float Reference::ToFloat() { return state_->ToFloat(index_); }

inline string_view Reference::ToString() { return state_->ToString(index_); }

inline CFunction Reference::ToCFunction() {
  return state_->ToCFunction(index_);
}

inline void* Reference::ToUserdata() { return state_->ToUserdata(index_); }

inline const void* Reference::ToPointer() { return state_->ToPointer(index_); }

inline void Reference::PushLength(bool raw) { state_->LEN(index_, raw); }

inline void Reference::PopUservalue() { state_->PopUservalue(index_); }

inline void Reference::PushUservalue() { state_->PushUservalue(index_); }

inline void Reference::PopField(bool raw) { state_->PopField(index_, raw); }

inline void Reference::PushField(bool raw) { state_->PushField(index_, raw); }

[[gnu::warn_unused_result]] inline bool Reference::Next() {
  return state_->Next(index_);
}

inline void Reference::PopMetatable() { state_->PopMetatable(index_); }

[[gnu::warn_unused_result]] inline bool Reference::PushMetatable() {
  return state_->PushMetatable(index_);
}

}  // namespace xxua
