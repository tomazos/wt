#include <stdio.h>

#include "gtest/gtest.h"

TEST(GTestTest, Smoke) { EXPECT_EQ(2 + 2, 4); }

GTEST_API_ int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
