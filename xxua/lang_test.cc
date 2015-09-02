#include "gtest/gtest.h"

#include "xxua/api.h"
#include "xxua/context.h"
#include "xxua/debug_allocator.h"
#include "xxua/state.h"

#include <boost/any.hpp>

namespace xxua {

TEST(LangTest, All) {
  const char* code = R"(
  local a, A, a_, Aa, Aa_;

  s = "a\a\b\f\n\r\t\v\\\"\'";

  function assert(condition) {
    if (!condition)
      error("assertion failed");
  }

  assert(1 == 1.00000);
  assert(05 == 5);
  assert(010 != 10);
  assert(010 == 8);
  assert(05342 == 5 * 8 * 8 * 8 + 3 * 8 * 8 + 4 * 8 + 2);
  assert(5342 == 5 * 10 * 10 * 10 + 3 * 10 * 10 + 4 * 10 + 2);
  assert(0x5C4a == 5 * 16 * 16 * 16 + (10 + 2) * 16 * 16 + 4 * 16 + (10));
  assert(0b0101'1100'0100'1010 == 0x5C4a);
  assert(02'4 == 024);
  assert(1'23'4 == 1234);
  assert(0x1234'5678'9ABC'DE'F0 == 0x123456789ABCDEF0);

  // logical operators
  assert(!null);
  assert(true);
  assert(!!true);
  assert(!false);

  assert(true && true);
  assert(!(true && false));
  assert(!(false && true));
  assert(!(false && false));

  assert(true || true);
  assert(true || false);
  assert(false || true);
  assert(!(false || false));

  // comparison
  assert(2 == 2);
  assert(!(2 == 3));

  assert(!(2 != 2));
  assert(2 != 3);

  assert(2 < 3);
  assert(!(2 < 2));
  assert(!(3 < 2));

  assert(!(2 > 3));
  assert(!(2 > 2));
  assert(3 > 2);

  assert(2 <= 3);
  assert(2 <= 2);
  assert(!(3 <= 2));

  assert(!(2 >= 3));
  assert(2 >= 2);
  assert(3 >= 2);

  // binary operations
  assert(0b1011'1010 | 0b0001'0101 == 0b1011'1111);
  assert(0b1011'1010 & 0b0001'0101 == 0b0001'0000);
  assert(0b1011'1010 ^ 0b0001'0101 == 0b1010'1111);
  assert(~0b1011'1010 & 0b1111'1111 == 0b0100'0101);

//  // shifting
  assert(0b101 << 3 == 0b101'000);
  assert(0b101'000 >> 3 == 0b101);

  assert(5 + 3 == 8);
  assert(5 - 3 == 2);
  assert(5 * 3 == 15);
  assert(11 % 3 == 2);

  assert(0 - 5 == -5);

  assert(7.6 / 2 == 3.8);
  assert(7.6 / 2.0 == 3.8);
  assert(9 / 2.0 == 4.5);
  assert(9 / 2 == 4);
  assert(8 / 2 == 4);

  assert(9 / 4 == 2);
  assert(9.0 / 4 == 2.25);

  local x = 3;
  assert(x == 3);
  {
    local x = 4;
    assert(x == 4);
  }
  assert(x == 3);

  local t = 0;
  function f() {
    t = t + 1;
  }

  x = 3;
  if (x == 3) {
    x = 6;
    local x = 4;
  }
  else
    local x = 5;
  assert(x == 6);

  while (t < 5)
    f();
  assert(t == 5);

  t = 0;
  local y = 0;
  while (t < 5) {
    t = t + 1;
    local y = 0;
    y = y + 1;
  }
  assert(t == 5);
  assert(y == 0);

  t = 0;
  assert(t == 0);
  do {
    t = t + 1;
    local t = 0;
    assert(t == 0);
  } while (t < 5);
  assert(t == 5);

  x = {1,2,3};
)";

  DebugAllocator allocator;
  State state(allocator);
  Context context(state);
  LoadFromString(code);
  PushGlobalTable();

  PushString("error");
  PushFunction([] {
    Concat(StackSize());
    Throw();
  });
  PopField(-3);

  Pop();
  Call(0, 0);

  PushGlobalTable();
  PushString("s");
  PushField(-2);
  EXPECT_EQ(ToString(-1), "a\a\b\f\n\r\t\v\\\"\'");
}

}  // namespace xxua
