#include "core/hex.h"

#include "gtest/gtest.h"

TEST(HexTest, HexCharToInt) {
  EXPECT_EQ(HexCharToInt('0'), 0x0);
  EXPECT_EQ(HexCharToInt('1'), 0x1);
  EXPECT_EQ(HexCharToInt('2'), 0x2);
  EXPECT_EQ(HexCharToInt('3'), 0x3);
  EXPECT_EQ(HexCharToInt('4'), 0x4);
  EXPECT_EQ(HexCharToInt('5'), 0x5);
  EXPECT_EQ(HexCharToInt('6'), 0x6);
  EXPECT_EQ(HexCharToInt('7'), 0x7);
  EXPECT_EQ(HexCharToInt('8'), 0x8);
  EXPECT_EQ(HexCharToInt('9'), 0x9);
  EXPECT_EQ(HexCharToInt('A'), 0xA);
  EXPECT_EQ(HexCharToInt('a'), 0xa);
  EXPECT_EQ(HexCharToInt('B'), 0xB);
  EXPECT_EQ(HexCharToInt('b'), 0xb);
  EXPECT_EQ(HexCharToInt('C'), 0xC);
  EXPECT_EQ(HexCharToInt('c'), 0xc);
  EXPECT_EQ(HexCharToInt('D'), 0xD);
  EXPECT_EQ(HexCharToInt('d'), 0xd);
  EXPECT_EQ(HexCharToInt('E'), 0xE);
  EXPECT_EQ(HexCharToInt('e'), 0xe);
  EXPECT_EQ(HexCharToInt('F'), 0xF);
  EXPECT_EQ(HexCharToInt('f'), 0xf);
  EXPECT_ANY_THROW(HexCharToInt('G'));
}

TEST(HexTest, IntToHexChar) {
  EXPECT_EQ(IntToHexChar(0x0), '0');
  EXPECT_EQ(IntToHexChar(0x1), '1');
  EXPECT_EQ(IntToHexChar(0x2), '2');
  EXPECT_EQ(IntToHexChar(0x3), '3');
  EXPECT_EQ(IntToHexChar(0x4), '4');
  EXPECT_EQ(IntToHexChar(0x5), '5');
  EXPECT_EQ(IntToHexChar(0x6), '6');
  EXPECT_EQ(IntToHexChar(0x7), '7');
  EXPECT_EQ(IntToHexChar(0x8), '8');
  EXPECT_EQ(IntToHexChar(0x9), '9');
  EXPECT_EQ(IntToHexChar(0xA), 'A');
  EXPECT_EQ(IntToHexChar(0xB), 'B');
  EXPECT_EQ(IntToHexChar(0xC), 'C');
  EXPECT_EQ(IntToHexChar(0xD), 'D');
  EXPECT_EQ(IntToHexChar(0xE), 'E');
  EXPECT_EQ(IntToHexChar(0xF), 'F');
  EXPECT_ANY_THROW(IntToHexChar(0xF + 1));
}

TEST(HexTest, HexStringConversions) {
  std::vector<uint8> bytes;

  bytes = HexStringToByteArray("");
  EXPECT_TRUE(bytes.empty());

  bytes = HexStringToByteArray("1234567890AbCdEf");
  EXPECT_EQ(bytes.size(), 8u);
  EXPECT_EQ(bytes[0], 0x12);
  EXPECT_EQ(bytes[1], 0x34);
  EXPECT_EQ(bytes[2], 0x56);
  EXPECT_EQ(bytes[3], 0x78);
  EXPECT_EQ(bytes[4], 0x90);
  EXPECT_EQ(bytes[5], 0xAB);
  EXPECT_EQ(bytes[6], 0xCD);
  EXPECT_EQ(bytes[7], 0xEF);

  EXPECT_EQ(ByteArrayToHexString(""), "");

  EXPECT_EQ(ByteArrayToHexString(bytes), "1234567890ABCDEF");
}
