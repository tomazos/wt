#include "core/regex.h"

#include "gtest/gtest.h"

TEST(RegexTest, Basic) {
  Regex ok("foo");
  bool did_throw;
  try {
    Regex notok("foo[");
    did_throw = false;
  } catch (const std::exception& e) {
    did_throw = true;
  }
  EXPECT_TRUE(did_throw);
}

TEST(RegexTest, Match) {
  Regex foo("foo");

  EXPECT_TRUE(foo.Matches("foobar"));
  EXPECT_FALSE(foo.Matches("barbaz"));
}

TEST(RegexTest, Capture) {
  Regex number_pattern(R"(\d+)");

  EXPECT_FALSE(number_pattern.Matches("fasjf"));
  EXPECT_TRUE(number_pattern.Matches("1234"));
  EXPECT_TRUE(number_pattern.Matches("fasfsa1234fasfgasg"));
  EXPECT_TRUE(number_pattern.Capture(
      "fasfsa1234fasfgasg",
      [](Regex::Results results) { EXPECT_EQ(results[0], "1234"); }));
}

TEST(RegexTest, Scan) {
  Regex number_pattern(R"((\d+))");
  std::vector<string> captures;

  size_t hits =
      number_pattern.Scan("123fasfas456gsagas789", [&](Regex::Results results) {
        captures.push_back(results[1].to_string());
        EXPECT_EQ(results[0], results[1]);
      });
  EXPECT_EQ(hits, 3u);
}
