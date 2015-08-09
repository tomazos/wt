#pragma once

#include <boost/multiprecision/cpp_int.hpp>

#include "core/string_functions.h"

class bigint {
 public:
  bigint() : m(0) {}
  bigint(int8 i) : m(i) {}
  bigint(int16 i) : m(i) {}
  bigint(int32 i) : m(i) {}
  bigint(int64 i) : m(i) {}

  bigint(uint8 i) : m(i) {}
  bigint(uint16 i) : m(i) {}
  bigint(uint32 i) : m(i) {}
  bigint(uint64 i) : m(i) {}

  template <typename T>
  explicit operator T() const {
    return m.template convert_to<T>();
  }

  bigint& operator++() {
    ++m;
    return *this;
  }
  bigint operator++(int) { return bigint(m++); }
  bigint& operator--() {
    --m;
    return *this;
  }
  bigint operator--(int) { return bigint(m--); }

  bigint operator+() const { return *this; }
  bigint operator-() const { return bigint(-m); }

  bigint operator<<(int x) const { return bigint(m << x); }
  bigint operator>>(int y) const { return bigint(m >> y); }

 private:
  bigint(boost::multiprecision::cpp_int m_in) : m(std::move(m_in)) {}

  boost::multiprecision::cpp_int m;

  friend inline bigint operator+(const bigint& a, const bigint& b);
  friend inline bigint operator-(const bigint& a, const bigint& b);
  friend inline bigint operator*(const bigint& a, const bigint& b);
  friend inline bigint operator/(const bigint& a, const bigint& b);
  friend inline bigint operator%(const bigint& a, const bigint& b);
  friend inline bigint operator==(const bigint& a, const bigint& b);
  friend inline bigint operator!=(const bigint& a, const bigint& b);
  friend inline bigint operator<(const bigint& a, const bigint& b);
  friend inline bigint operator>(const bigint& a, const bigint& b);
  friend inline bigint operator<=(const bigint& a, const bigint& b);
  friend inline bigint operator>=(const bigint& a, const bigint& b);
};

inline string EncodeAsString(bigint i) {
  return EncodeSignedIntegerAsString(i);
}

inline bigint operator+(const bigint& a, const bigint& b) {
  return bigint(a.m + b.m);
}
inline bigint operator-(const bigint& a, const bigint& b) {
  return bigint(a.m - b.m);
}
inline bigint operator*(const bigint& a, const bigint& b) {
  return bigint(a.m * b.m);
}
inline bigint operator/(const bigint& a, const bigint& b) {
  return bigint(a.m / b.m);
}
inline bigint operator%(const bigint& a, const bigint& b) {
  return bigint(a.m % b.m);
}
inline bigint operator==(const bigint& a, const bigint& b) {
  return bigint(a.m == b.m);
}
inline bigint operator!=(const bigint& a, const bigint& b) {
  return bigint(a.m != b.m);
}
inline bigint operator<(const bigint& a, const bigint& b) {
  return bigint(a.m < b.m);
}
inline bigint operator>(const bigint& a, const bigint& b) {
  return bigint(a.m > b.m);
}
inline bigint operator<=(const bigint& a, const bigint& b) {
  return bigint(a.m <= b.m);
}
inline bigint operator>=(const bigint& a, const bigint& b) {
  return bigint(a.m >= b.m);
}
