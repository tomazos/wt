#pragma once

#include <boost/multiprecision/cpp_int.hpp>

#include "core/string_functions.h"

class bigint {
 public:
  bigint() : m(0) {}
  bigint(int8 i) : m(i) {}
  bigint(int16 i) : m(i) {}
  bigint(int32 i) : m(i) {}
  bigint(long i) : m(i) {}
  bigint(int64 i) : m(i) {}

  bigint(uint8 i) : m(i) {}
  bigint(uint16 i) : m(i) {}
  bigint(uint32 i) : m(i) {}
  bigint(unsigned long i) : m(i) {}
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
  bigint operator>>(int x) const { return bigint(m >> x); }
  bigint& operator<<=(int x) {
    m <<= x;
    return *this;
  }
  bigint& operator>>=(int x) {
    m >>= x;
    return *this;
  }

#define DEFINE_BIGINT_ASSIGNOP(op)         \
  bigint& operator op(const bigint& rhs) { \
    m op rhs.m;                            \
    return *this;                          \
  }

  DEFINE_BIGINT_ASSIGNOP(+= )
  DEFINE_BIGINT_ASSIGNOP(-= )
  DEFINE_BIGINT_ASSIGNOP(*= )
  DEFINE_BIGINT_ASSIGNOP(/= )
  DEFINE_BIGINT_ASSIGNOP(%= )
  DEFINE_BIGINT_ASSIGNOP(&= )
  DEFINE_BIGINT_ASSIGNOP(|= )

 private:
  bigint(boost::multiprecision::cpp_int m_in) : m(std::move(m_in)) {}

  boost::multiprecision::cpp_int m;

  friend inline bigint operator+(const bigint& a, const bigint& b);
  friend inline bigint operator-(const bigint& a, const bigint& b);
  friend inline bigint operator*(const bigint& a, const bigint& b);
  friend inline bigint operator/(const bigint& a, const bigint& b);
  friend inline bigint operator%(const bigint& a, const bigint& b);
  friend inline bool operator==(const bigint& a, const bigint& b);
  friend inline bool operator!=(const bigint& a, const bigint& b);
  friend inline bool operator<(const bigint& a, const bigint& b);
  friend inline bool operator>(const bigint& a, const bigint& b);
  friend inline bool operator<=(const bigint& a, const bigint& b);
  friend inline bool operator>=(const bigint& a, const bigint& b);
};

inline string EncodeAsString(bigint i) {
  return EncodeSignedIntegerAsString(i);
}

#define DEFINE_BIGINT_BINOP(op)                                 \
  inline bigint operator op(const bigint& a, const bigint& b) { \
    return bigint(a.m op b.m);                                  \
  }

DEFINE_BIGINT_BINOP(+)
DEFINE_BIGINT_BINOP(-)
DEFINE_BIGINT_BINOP(*)
DEFINE_BIGINT_BINOP(/ )
DEFINE_BIGINT_BINOP(% )

#define DEFINE_BIGINT_RELOP(op)                               \
  inline bool operator op(const bigint& a, const bigint& b) { \
    return a.m op b.m;                                        \
  }

DEFINE_BIGINT_RELOP(== )
DEFINE_BIGINT_RELOP(!= )
DEFINE_BIGINT_RELOP(< )
DEFINE_BIGINT_RELOP(> )
DEFINE_BIGINT_RELOP(<= )
DEFINE_BIGINT_RELOP(>= )
