#include "core/bigint.h"

#include "gtest/gtest.h"

TEST(BigIntTest, Simple) {
  bigint i = 0;
  EXPECT_EQ(0, i);
  EXPECT_EQ(0, i++);
  EXPECT_EQ(1, i);
  EXPECT_EQ(2, ++i);
  EXPECT_EQ(1, --i);
  EXPECT_EQ(1, i);
  EXPECT_EQ(1, i--);
  EXPECT_EQ(0, i);
}

TEST(BigIntTest, Pack) {
  EXPECT_EQ(pack_bigint(0), string({0}));
  EXPECT_EQ(pack_bigint(1), string({1}));
  EXPECT_EQ(pack_bigint(0x7F), string({0x7F}));
  EXPECT_EQ(pack_bigint(0x80), string({char(0x81), char(0x00)}));
  EXPECT_EQ(pack_bigint(0x85), string({char(0x81), char(0x05)}));
  EXPECT_EQ(pack_bigint(0b1001'1110'1111'1000'0101),
            string({char(0b1'0100111), char(0b1'1011111), char(0b0'0000101)}));
}

struct StringGetter {
  StringGetter(string_view sv) : s(sv.to_string()) {}
  optional<uint8> operator()() {
    if (i == s.size())
      return nullopt;
    else
      return uint8(s[i++]);
  }
  std::string s;
  size_t i = 0;
};

static optional<bigint> UnpackString(string_view sv) {
  StringGetter sg(sv);
  return unpack_bigint(sg);
}

TEST(BigIntTest, Unpack) {
  for (bigint i = 0; i < 1000; i++) {
    EXPECT_EQ(i, UnpackString(pack_bigint(i)));
  }
}
