#pragma once

#include <postgresql/libpq-fe.h>

#include "core/must.h"

namespace database {
namespace postgresql {

template <typename T, Oid canonical_oid>
struct SimpleType {
  static constexpr Oid oid = canonical_oid;
  static constexpr int format = 1;
  static constexpr int length(T) { return sizeof(T); }
  struct Encoded {
    Encoded(T) {}
    const void* get(const T& t) const { return &t; }
  };
  static T Decode(string_view data, Oid oid_in) {
    if (oid_in != canonical_oid)
      FAIL("Type mismatch: ", oid_in, " vs ", canonical_oid);
    if (data.size() != sizeof(T))
      FAIL("Unexpected data size: ", data.size(), " vs ", sizeof(T));
    T t;
    std::memcpy(&t, data.data(), sizeof(T));
    return t;
  }
};

template <typename T, Oid canonical_oid>
struct ByteReversedType {
  static constexpr Oid oid = canonical_oid;
  static constexpr int format = 1;
  static constexpr int length(T) { return sizeof(T); }
  static constexpr size_t N = sizeof(T);
  struct Encoded {
    int8 c[N];
    Encoded(T t) {
      int8* p = (int8*)&t;
      for (size_t i = 0; i < N; ++i) {
        const size_t j = N - i - 1;
        c[j] = p[i];
      }
    }
    const void* get(const T&) const { return c; }
  };
  static T Decode(string_view data, Oid oid_in) {
    if (oid_in != canonical_oid)
      FAIL("Type mismatch: ", oid_in, " vs ", canonical_oid);
    if (data.size() != sizeof(T))
      FAIL("Unexpected data size: ", data.size(), " vs ", sizeof(T));
    T t;
    static constexpr size_t N = sizeof(T);
    int8* p = (int8*)&t;
    for (size_t i = 0; i < N; ++i) {
      const size_t j = N - i - 1;
      p[j] = data[i];
    }

    return t;
  }
};

template <typename T, Oid canonical_oid>
struct StringType {
  static constexpr Oid oid = canonical_oid;
  static constexpr int format = 1;
  static constexpr int length(const T& s) { return s.size(); }
  struct Encoded {
    Encoded(const T&) {}
    const void* get(const T& s) const { return s.data(); }
  };
  static T Decode(string_view data, Oid oid_in) {
    if (oid_in != canonical_oid)
      FAIL("Type mismatch: ", oid_in, " vs ", canonical_oid);
    return data.to_string();
  }
};

template <typename T, Oid canonical_oid>
struct ByteaType {
  static constexpr Oid oid = canonical_oid;
  static constexpr int format = 1;
  static constexpr int length(const T& s) { return s.data.size(); }
  struct Encoded {
    Encoded(const T&) {}
    const void* get(const T& s) const { return s.data.data(); }
  };
  static T Decode(string_view data, Oid oid_in) {
    if (oid_in != canonical_oid)
      FAIL("Type mismatch: ", oid_in, " vs ", canonical_oid);
    return {data.to_string()};
  }
};

// template <typename T, Oid canonical_oid>
// struct StringType {
//  static constexpr Oid oid = canonical_oid;
//  static constexpr int format = 1;
//  static constexpr int length(const T& s) { return s.size(); }
//  struct Encoded {
//    Encoded(const T&) {}
//    const void* get(const T& s) const { return s.data(); }
//  };
//  static T Decode(string_view data, Oid oid_in) {
//    if (oid_in != canonical_oid && oid_in != 25 /*text*/)
//      FAIL("Type mismatch: ", oid_in, " vs ", canonical_oid);
//    return data.to_string();
//  }
//};

template <typename T>
struct Type;

template <>
struct Type<bool> : SimpleType<bool, 16 /*oid*/> {};

template <>
struct Type<char> : SimpleType<char, 18 /*oid*/> {};

template <>
struct Type<int16> : ByteReversedType<int16, 21 /*oid*/> {};

template <>
struct Type<int32> : ByteReversedType<int32, 23 /*oid*/> {};

template <>
struct Type<int64> : ByteReversedType<int64, 20 /*oid*/> {};

template <>
struct Type<float32> : ByteReversedType<float32, 700 /*oid*/> {};

template <>
struct Type<float64> : ByteReversedType<float64, 701 /*oid*/> {};

template <>
struct Type<string> : StringType<string, 25 /*oid*/> {};

struct bytea {
  string data;
  bool operator==(const bytea& rhs) const { return data == rhs.data; }
};

template <>
struct Type<bytea> : ByteaType<bytea, 17 /*oid*/> {};

}  // namespace postgresql
}  // namespace database
