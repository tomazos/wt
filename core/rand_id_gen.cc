#include "core/random.h"
#include "main/noargs.h"

#include <cmath>
#include <iostream>

void Main() {
  for (int i = 0; i < 1000; i++) {
    int x = std::floor((10 + 26 * 2) * RandFloat());
    if (x < 10) {
      std::cout << char('0' + x);
    } else if (x < 10 + 26) {
      std::cout << char('a' + x - 10);
    } else {
      std::cout << char('A' + x - 10 - 26);
    }
  }
  std::cout.flush();
}
