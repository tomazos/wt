#include "ladybug/ladybug.h"

#include <iostream>

#include "must.h"

int main(int argc, char** argv) {
  try {
    std::vector<string> args(argv + 1, argv + argc);
    if (args.size() < 2 || (args[0] != "L" && args[0] != "R" &&
                            args[0] != "B" && args[0] != "O")) {
      FAIL("Usage: ladybug_motor_set <L|R|B|O> <-1.0..+1.0>");
    }

    LadyBug ladybug;

    float64 power = std::stod(args[1]);
    if (args[0] == "L") {
      std::cout << "Applying " << power << " to left wheel" << std::endl;
      ladybug.SetLeftWheelPower(power);
    } else if (args[0] == "R") {
      std::cout << "Applying " << power << " to right wheel" << std::endl;
      ladybug.SetRightWheelPower(power);
    } else if (args[0] == "B") {
      std::cout << "Applying " << power << " to both wheels" << std::endl;
      ladybug.SetLeftWheelPower(power);
      ladybug.SetRightWheelPower(power);
    } else {
      std::cout << "Applying " << power << " to left and " << -power
                << " to right" << std::endl;
      ladybug.SetLeftWheelPower(power);
      ladybug.SetRightWheelPower(-power);
    }

    std::getchar();

  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    std::exit(1);
  }
}
