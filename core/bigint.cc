#include "core/bigint.h"

std::string pack_bigint(bigint n) {
  std::vector<uint8> ne;
  do {
    uint8 m = (uint8(n) & 0b0111'1111);
    n >>= 7;
    m |= 0b1000'0000;
    ne.push_back(m);
  } while (n > 0);
  Reverse(ne);
  ne.back() &= 0b0111'1111;
  const std::string s((const char*)ne.data(), ne.size());
  return s;
}
