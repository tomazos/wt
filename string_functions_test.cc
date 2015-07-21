#include "string_functions.h"

#include <cstdio>
#include <limits>

#define CHECK_STRING(expected, ...)                      \
  {                                                      \
    const string observed = EncodeAsString(__VA_ARGS__); \
    if (expected != observed) {                          \
      std::printf("expected: %s\n", expected);           \
      std::printf("observed: %s\n", observed.c_str());   \
      std::exit(1);                                      \
    }                                                    \
  }

int main() {
  const string foo = "foo";
  CHECK_STRING("foo", foo);
  CHECK_STRING("foo", "foo");

  CHECK_STRING("0", 0);
  CHECK_STRING("1", 1);
  CHECK_STRING("-1", -1);
  CHECK_STRING("123", 123);
  CHECK_STRING("-123", -123);

  CHECK_STRING("-128", std::numeric_limits<int8>::min());
  CHECK_STRING("127", std::numeric_limits<int8>::max());
  CHECK_STRING("-32768", std::numeric_limits<int16>::min());
  CHECK_STRING("32767", std::numeric_limits<int16>::max());
  CHECK_STRING("-2147483648", std::numeric_limits<int32>::min());
  CHECK_STRING("2147483647", std::numeric_limits<int32>::max());
  CHECK_STRING("-9223372036854775808", std::numeric_limits<int64>::min());
  CHECK_STRING("9223372036854775807", std::numeric_limits<int64>::max());
  //  CHECK_STRING("-170141183460469231731687303715884105728",
  //  std::numeric_limits<int128>::min());
  //  CHECK_STRING("170141183460469231731687303715884105727",
  //  std::numeric_limits<int128>::max());

  CHECK_STRING("0", std::numeric_limits<uint8>::min());
  CHECK_STRING("255", std::numeric_limits<uint8>::max());
  CHECK_STRING("0", std::numeric_limits<uint16>::min());
  CHECK_STRING("65535", std::numeric_limits<uint16>::max());
  CHECK_STRING("0", std::numeric_limits<uint32>::min());
  CHECK_STRING("4294967295", std::numeric_limits<uint32>::max());
  CHECK_STRING("0", std::numeric_limits<uint64>::min());
  CHECK_STRING("18446744073709551615", std::numeric_limits<uint64>::max());
  //  CHECK_STRING("0", std::numeric_limits<uint128>::min());
  //  CHECK_STRING("340282366920938463463374607431768211455",
  //  std::numeric_limits<uint128>::max());

  CHECK_STRING("123", bigint(123));

  CHECK_STRING("123456789", 123, "456", bigint(789));
}
