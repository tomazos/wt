#include "testing.h"

#include <iostream>

int main() {
  try {
    TestMain();
  } catch (const Error& error) {
    std::cerr << "TEST FAILED: " << error.GetErrorMessage() << std::endl;
  }
}
