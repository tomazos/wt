#pragma once

#include "xxua/context.h"
#include "xxua/pointer.h"
#include "xxua/reference.h"
#include "xxua/state.h"

namespace xxua {

class Stack {
 public:
  inline Stack();
  inline Stack(State& state);

  inline Reference front();
  inline Reference back();

  inline Reference operator[](int pos);

  inline Pointer begin();
  inline Pointer end();

  inline bool empty();
  inline int size();

  inline void clear();

  inline void resize(int count);

 private:
  State* state_;
};

inline Reference Registry();
inline Reference Registry(State& state);

inline Stack::Stack() : state_(Context::Current()) {}
inline Stack::Stack(State& state) : state_(&state) {}

inline Reference Stack::front() { return {state_, 1}; }
inline Reference Stack::back() { return {state_, state_->StackSize()}; }

inline Reference Stack::operator[](int pos) { return {state_, pos + 1}; }

inline Pointer Stack::begin() { return {state_, 1}; }
inline Pointer Stack::end() { return {state_, state_->StackSize() + 1}; }

inline bool Stack::empty() { return state_->StackSize() == 0; }
inline int Stack::size() { return state_->StackSize(); }

inline void Stack::clear() { state_->ResizeStack(0); }

inline void Stack::resize(int count) { state_->ResizeStack(count); }

inline Reference Registry() { return Registry(*Context::Current()); }
inline Reference Registry(State& state) { return {&state, State::REGISTRY}; }

}  // namespace xxua
