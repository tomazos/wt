#include "testing.h"

#include "core/must.h"

void TestMain() {
  union {
    uint16 word;
    struct {
      uint8 lo;
      uint8 hi;
    } bytes;
  } u;
  STATIC_ASSERT(sizeof(u) == 2);
  u.word = 0x0102;
  MUST_EQ(0x02, u.bytes.lo);
  MUST_EQ(0x01, u.bytes.hi);
}
