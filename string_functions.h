#pragma once

#include <cctype>
#include <vector>
#include <sstream>

#include "collection_functions.h"

inline bool IsSpace(char c) { return std::isspace(c); }

inline string EncodeAsString(string str);
inline string EncodeAsString(const char* str);
inline string EncodeAsString(int8 i);
inline string EncodeAsString(int16 i);
inline string EncodeAsString(int32 i);
inline string EncodeAsString(int64 i);

inline string EncodeAsString(uint8 i);
inline string EncodeAsString(uint16 i);
inline string EncodeAsString(uint32 i);
inline string EncodeAsString(uint64 i);

inline string EncodeAsString(string str) { return std::move(str); }

inline string EncodeAsString(const char* str) { return str; }

template <typename T>
string EncodeAsString(const T& val) {
  std::ostringstream oss;
  oss << val;
  return oss.str();
}

template <typename Arg0, typename Arg1, typename... Args>
string EncodeAsString(Arg0&& arg0, Arg1&& arg1, Args&&... args) {
  const string strs[] = {EncodeAsString(std::forward<Arg0>(arg0)),
                         EncodeAsString(std::forward<Arg1>(arg1)),
                         EncodeAsString(std::forward<Args>(args))...};
  size_t length = 0;
  for (const string& str : strs) {
    length += str.size();
  }
  string result(length, '0');
  auto pos_iter = result.begin();
  for (const string& str : strs) {
    pos_iter = CopyCollection(str, pos_iter);
  }
  return result;
}

template <typename I>
string EncodeNegativeIntegerAsString(I i) {
  std::vector<char> digits;
  do {
    const I j = i / I(10);
    const I k = i - j * I(10);
    digits.push_back('0' + char(-k));
    i = j;
  } while (i < 0);
  digits.push_back('-');
  return string(digits.crbegin(), digits.crend());
}

template <typename I>
string EncodePositiveIntegerAsString(I i) {
  std::vector<char> digits;
  do {
    const I j = i / I(10);
    const I k = i - j * I(10);
    digits.push_back('0' + char(k));
    i = j;
  } while (i > 0);
  return string(digits.crbegin(), digits.crend());
}

template <typename I>
string EncodeSignedIntegerAsString(I i) {
  if (i > I(0))
    return EncodePositiveIntegerAsString(i);
  else if (i == I(0))
    return "0";
  else  // i < I(0)
    return EncodeNegativeIntegerAsString(i);
}

template <typename I>
string EncodeUnsignedIntegerAsString(I i) {
  if (i > I(0))
    return EncodePositiveIntegerAsString(i);
  else
    return "0";
}

inline string EncodeAsString(int8 i) { return EncodeSignedIntegerAsString(i); }

inline string EncodeAsString(int16 i) { return EncodeSignedIntegerAsString(i); }

inline string EncodeAsString(int32 i) { return EncodeSignedIntegerAsString(i); }

inline string EncodeAsString(int64 i) { return EncodeSignedIntegerAsString(i); }

inline string EncodeAsString(uint8 i) {
  return EncodeUnsignedIntegerAsString(i);
}

inline string EncodeAsString(uint16 i) {
  return EncodeUnsignedIntegerAsString(i);
}

inline string EncodeAsString(uint32 i) {
  return EncodeUnsignedIntegerAsString(i);
}

inline string EncodeAsString(uint64 i) {
  return EncodeUnsignedIntegerAsString(i);
}
