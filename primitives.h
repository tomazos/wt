#pragma once

#include <chrono>
#include <cstdint>
#include <cstring>
#include <experimental/optional>
#include <experimental/string_view>
#include <mutex>
#include <string>
#include <type_traits>
#include <thread>
#include <sys/time.h>

#define STATIC_ASSERT(cond) static_assert(cond, #cond);

using std::string;
using std::experimental::string_view;

using Mutex = std::mutex;
using LockGuard = std::lock_guard<std::mutex>;

using namespace std::chrono_literals;

template <typename Time>
void Sleep(Time amount) {
  std::this_thread::sleep_for(amount);
}

inline auto Now() { return std::chrono::high_resolution_clock::now(); }

using std::experimental::optional;
using std::experimental::nullopt;
using std::experimental::in_place;
using std::experimental::make_optional;

namespace google {}
using namespace google;

namespace boost {
namespace algorithm {}
namespace filesystem {}
};
namespace algorithm = boost::algorithm;
namespace filesystem = boost::filesystem;

using int8 = std::int8_t;
using int16 = std::int16_t;
using int32 = std::int32_t;
using int64 = long long;
// using int128 = __int128;
using uint8 = std::uint8_t;
using uint16 = std::uint16_t;
using uint32 = std::uint32_t;
using uint64 = unsigned long long;
// using uint128 = unsigned __int128;

using float32 = float;
using float64 = double;

STATIC_ASSERT(sizeof(int8) == 1);
STATIC_ASSERT(sizeof(int16) == 2);
STATIC_ASSERT(sizeof(int32) == 4);
STATIC_ASSERT(sizeof(int64) == 8);
// STATIC_ASSERT(sizeof(int128) == 16);

STATIC_ASSERT(sizeof(uint8) == 1);
STATIC_ASSERT(sizeof(uint16) == 2);
STATIC_ASSERT(sizeof(uint32) == 4);
STATIC_ASSERT(sizeof(uint64) == 8);
// STATIC_ASSERT(sizeof(uint128) == 16);

STATIC_ASSERT(sizeof(float32) == 4);
STATIC_ASSERT(sizeof(float64) == 8);

STATIC_ASSERT(std::is_signed<int8>::value);
STATIC_ASSERT(std::is_signed<int16>::value);
STATIC_ASSERT(std::is_signed<int32>::value);
STATIC_ASSERT(std::is_signed<int64>::value);
// TODO(zos): Specialize numeric_limits etc for int128
// STATIC_ASSERT(std::is_signed<int128>::value);

STATIC_ASSERT(std::is_unsigned<uint8>::value);
STATIC_ASSERT(std::is_unsigned<uint16>::value);
STATIC_ASSERT(std::is_unsigned<uint32>::value);
STATIC_ASSERT(std::is_unsigned<uint64>::value);
// TODO(zos): Specialize numeric_limits etc for uint128
// STATIC_ASSERT(std::is_unsigned<uint128>::value);

inline float64 now_secs() {
  timeval tv;
  std::memset(&tv, 0, sizeof tv);
  int gettimeofday_result = gettimeofday(&tv, nullptr);
  if (gettimeofday_result != 0) throw std::logic_error("gettimeofday");
  return float64(tv.tv_sec) + float64(tv.tv_usec) / 1'000'000;
}
