#include "main/noargs.h"

#include <cstdlib>
#include <iostream>

int main() {
  try {
    Main();
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }
}
