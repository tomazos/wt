#include "main/args.h"

#include <cstdlib>
#include <iostream>

int main(int argc, char** argv) {
  std::vector<string> args(argv + 1, argv + argc);
  try {
    Main(args);
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }
}
