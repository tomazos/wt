#include "ladybug/ladybug.h"
#include "main/noargs.h"

#include <cmath>
#include <iostream>

float64 Sigmoid(float64 x) { return 1.0 / (1.0 + std::exp(-x)); }

float64 InvSigmoid(float64 y) {
  if (y <= 0) {
    return -INFINITY;
  } else if (y >= 1) {
    return +INFINITY;
  } else {
    return -std::log(1.0 / y - 1.0);
  }
}

float64 Adjust(float64 power, float64 dps) {
  return Sigmoid(InvSigmoid(power) + (Sigmoid(dps) - 0.5) / 100);
}

void Main() {
  LadyBug& ladybug = LadyBug::Instance();
  std::atomic<bool> shutdown;
  std::thread t([&] {
    ladybug.SetLeftWheelPower(0.5);
    ladybug.SetRightWheelPower(0.5);

    while (!shutdown) {
      auto state = ladybug.GetState();
      DUMPEXPR(state.gz());
      DUMPEXPR(state.left());
      DUMPEXPR(state.right());

      if (state.sonar() != 0 && state.sonar() < 0.3) {
        std::cout << "OBJECT NEAR" << std::endl;
        ladybug.SetLeftWheelPower(0);
        ladybug.SetRightWheelPower(0);
        return;
      } else {
        ladybug.SetLeftWheelPower(Adjust(state.left(), state.gz()));
        ladybug.SetRightWheelPower(Adjust(state.right(), -state.gz()));
      }

      Sleep(10ms);
    }
  });

  std::getchar();
  shutdown = true;
  t.join();
}
