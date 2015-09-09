#include "core/env.h"

#include "gtest/gtest.h"

TEST(EnvTest, Basic) {
  EXPECT_FALSE(HasEnv("FOO"));
  SetEnv("FOO", "BAR");
  EXPECT_TRUE(HasEnv("FOO"));
  EXPECT_EQ(GetEnv("FOO"), "BAR");
  SetEnv("FOO", "BAZ", false /*overwrite*/);
  EXPECT_EQ(GetEnv("FOO"), "BAR");
  SetEnv("FOO", "BAZ");
  EXPECT_EQ(GetEnv("FOO"), "BAZ");
  EXPECT_ANY_THROW(GetEnv("gfagkiasngfansga"));
}
