#pragma once

#include "xxua/state.h"
#include "xxua/reference.h"

namespace xxua {

class Pointer {
 public:
  inline Reference operator*();

  inline Pointer& operator++();
  inline Pointer operator++(int);
  inline Pointer& operator--();
  inline Pointer operator--(int);

  inline Pointer& operator+=(int d);
  inline Pointer& operator-=(int d);

 private:
  Pointer(State* state, Index index)
      : state_(state), index_(state->AbsIndex(index)) {}

  State* state_;
  Index index_;

  friend class Stack;

  friend inline Pointer operator+(Pointer p, int d);
  friend inline Pointer operator+(int d, Pointer p);

  friend inline Pointer operator-(Pointer p, int d);
  friend inline int operator-(Pointer p, Pointer q);

  friend inline bool operator==(Pointer p, Pointer q);
  friend inline bool operator!=(Pointer p, Pointer q);
  friend inline bool operator<(Pointer p, Pointer q);
  friend inline bool operator>(Pointer p, Pointer q);
  friend inline bool operator<=(Pointer p, Pointer q);
  friend inline bool operator>=(Pointer p, Pointer q);
};

inline Pointer operator+(Pointer p, int d);
inline Pointer operator+(int d, Pointer p);

inline Pointer operator-(Pointer p, int d);
inline int operator-(Pointer p, Pointer q);

inline bool operator==(Pointer p, Pointer q);
inline bool operator!=(Pointer p, Pointer q);
inline bool operator<(Pointer p, Pointer q);
inline bool operator>(Pointer p, Pointer q);
inline bool operator<=(Pointer p, Pointer q);
inline bool operator>=(Pointer p, Pointer q);

inline Pointer& Pointer::operator++() {
  ++index_;
  return *this;
}

inline Pointer Pointer::operator++(int) { return {state_, index_++}; }

inline Pointer& Pointer::operator--() {
  --index_;
  return *this;
}

inline Pointer Pointer::operator--(int) { return {state_, index_--}; }

inline Pointer& Pointer::operator+=(int d) {
  index_ += d;
  return *this;
}
inline Pointer& Pointer::operator-=(int d) {
  index_ -= d;
  return *this;
}

inline Reference Pointer::operator*() { return {state_, index_}; }

inline Pointer operator+(Pointer p, int d) { return {p.state_, p.index_ + d}; }
inline Pointer operator+(int d, Pointer p) { return p + d; }

inline Pointer operator-(Pointer p, int d) { return p + (-d); }
inline int operator-(Pointer p, Pointer q) { return p.index_ - q.index_; }

inline bool operator==(Pointer p, Pointer q) {
  return p.state_ == q.state_ && p.index_ == q.index_;
}

inline bool operator!=(Pointer p, Pointer q) { return !(p == q); }

inline bool operator<(Pointer p, Pointer q) {
  if (p.state_ == q.state_)
    return p.index_ < q.index_;
  else
    return p.state_ < q.state_;
}

inline bool operator>(Pointer p, Pointer q) { return q < p; }
inline bool operator<=(Pointer p, Pointer q) { return !(p > q); }
inline bool operator>=(Pointer p, Pointer q) { return !(p < q); }

}  // namespace xxua
