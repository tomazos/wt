#include "core/utf8.h"

#include "core/must.h"
#include "gtest/gtest.h"

template <typename... Args>
static string str(Args&&... args) {
  char data[] = {(char)((uint8)(args))...};
  return string(data, data + sizeof(data));
}

TEST(Utf8Test, IsValidUtf8) {
  EXPECT_TRUE(IsValidUtf8(str(0b00000000)));
  EXPECT_TRUE(IsValidUtf8(str(0b01000000)));
  EXPECT_TRUE(IsValidUtf8(str(0b01111111)));
  EXPECT_FALSE(IsValidUtf8(str(0b10000000)));
  EXPECT_FALSE(IsValidUtf8(str(0b11111111)));
  EXPECT_TRUE(IsValidUtf8(str(0b00000000, 0b01000000, 0b01111111)));
  EXPECT_FALSE(
      IsValidUtf8(str(0b00000000, 0b01000000, 0b01111111, 0b10000000)));
  EXPECT_FALSE(
      IsValidUtf8(str(0b00000000, 0b01000000, 0b01111111, 0b11111111)));
  EXPECT_FALSE(
      IsValidUtf8(str(0b00000000, 0b01000000, 0b10000000, 0b01111111)));
  EXPECT_FALSE(
      IsValidUtf8(str(0b00000000, 0b01000000, 0b11111111, 0b01111111)));
  EXPECT_FALSE(
      IsValidUtf8(str(0b10000000, 0b00000000, 0b01000000, 0b01111111)));
  EXPECT_FALSE(
      IsValidUtf8(str(0b11111111, 0b00000000, 0b01000000, 0b01111111)));

  EXPECT_TRUE(IsValidUtf8(str(0b1100'0000, 0b1000'0000)));
  EXPECT_TRUE(IsValidUtf8(str(0b1101'1111, 0b1011'1111)));
  EXPECT_FALSE(IsValidUtf8(str(0b1100'0000, 0b0000'0000)));
  EXPECT_FALSE(IsValidUtf8(str(0b1101'1111, 0b0011'1111)));
  EXPECT_FALSE(IsValidUtf8(str(0b1100'0000)));
  EXPECT_FALSE(IsValidUtf8(str(0b1101'1111)));

  EXPECT_TRUE(IsValidUtf8(str(0b1110'0000, 0b1000'0000, 0b1000'0000)));
  EXPECT_TRUE(IsValidUtf8(str(0b1110'1111, 0b1011'1111, 0b1011'1111)));
  EXPECT_FALSE(IsValidUtf8(str(0b1110'0000, 0b1000'0000, 0b0000'0000)));
  EXPECT_FALSE(IsValidUtf8(str(0b1110'1111, 0b1011'1111, 0b0011'1111)));
  EXPECT_FALSE(IsValidUtf8(str(0b1110'0000, 0b1000'0000)));
  EXPECT_FALSE(IsValidUtf8(str(0b1110'1111, 0b1011'1111)));
  EXPECT_FALSE(IsValidUtf8(str(0b1110'0000, 0b0000'0000)));
  EXPECT_FALSE(IsValidUtf8(str(0b1110'1111, 0b0011'1111)));
  EXPECT_FALSE(IsValidUtf8(str(0b1110'0000)));
  EXPECT_FALSE(IsValidUtf8(str(0b1110'1111)));

  EXPECT_TRUE(
      IsValidUtf8(str(0b1111'0000, 0b1000'0000, 0b1000'0000, 0b1000'0000)));
  EXPECT_TRUE(
      IsValidUtf8(str(0b1111'0111, 0b1011'1111, 0b1011'1111, 0b1000'0000)));
  EXPECT_FALSE(
      IsValidUtf8(str(0b1111'0000, 0b1000'0000, 0b1000'0000, 0b0000'0000)));
  EXPECT_FALSE(
      IsValidUtf8(str(0b1111'0111, 0b1011'1111, 0b1011'1111, 0b0000'0000)));
  EXPECT_FALSE(IsValidUtf8(str(0b1111'0000, 0b1000'0000, 0b1000'0000)));
  EXPECT_FALSE(IsValidUtf8(str(0b1111'0111, 0b1011'1111, 0b1011'1111)));
  EXPECT_FALSE(IsValidUtf8(str(0b1111'0000, 0b1000'0000, 0b0000'0000)));
  EXPECT_FALSE(IsValidUtf8(str(0b1111'0111, 0b1011'1111, 0b0011'1111)));
  EXPECT_FALSE(IsValidUtf8(str(0b1111'0000, 0b1000'0000)));
  EXPECT_FALSE(IsValidUtf8(str(0b1111'0111, 0b1011'1111)));
  EXPECT_FALSE(IsValidUtf8(str(0b1111'0000, 0b0000'0000)));
  EXPECT_FALSE(IsValidUtf8(str(0b1111'0111, 0b0011'1111)));
  EXPECT_FALSE(IsValidUtf8(str(0b1111'0000)));
  EXPECT_FALSE(IsValidUtf8(str(0b1111'0111)));
}

TEST(Utf8Test, ConvertLatin1ToUtf8) {
  EXPECT_EQ(ConvertLatin1ToUtf8(str(0b0000'0000)), str(0b0000'0000));
  EXPECT_EQ(ConvertLatin1ToUtf8(str(0b0111'1111)), str(0b0111'1111));
  EXPECT_EQ(ConvertLatin1ToUtf8(str(0b1000'0000)),
            str(0b1100'0010, 0b1000'0000));
  EXPECT_EQ(ConvertLatin1ToUtf8(str(0b1111'1111)),
            str(0b1100'0011, 0b1011'1111));
  EXPECT_EQ(ConvertLatin1ToUtf8(str(0b1100'1010)),
            str(0b1100'0011, 0b1000'1010));
}
