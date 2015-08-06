#include "ladybug/ladybug.h"

#include <iostream>

int main() {
  try {
    LadyBug ladybug;
    ladybug.AddSonarListener(
        [](float64 meters) { std::cout << meters * 1000 << "mm" << std::endl; });

    std::getchar();
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    std::exit(1);
  }
}
