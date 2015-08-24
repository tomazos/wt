#include "testing.h"

#include <iostream>

int main() {
  try {
    TestMain();
  } catch (const Error& error) {
    std::cerr << error.what() << std::endl;
  }
}
