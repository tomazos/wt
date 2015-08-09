#include "core/bigint.h"

#include "testing.h"

void TestMain() {
  bigint i = 0;
  MUST_EQ(0, i);
  MUST_EQ(0, i++);
  MUST_EQ(1, i);
  MUST_EQ(2, ++i);
  MUST_EQ(1, --i);
  MUST_EQ(1, i);
  MUST_EQ(1, i--);
  MUST_EQ(0, i);
}
