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
  return Sigmoid(InvSigmoid(power) + (Sigmoid(dps) - 0.5) / 50);
}

float64 Angle(float64 x, float64 y) {
  float64 nx = x / std::hypot(x, y);
  float64 ny = y / std::hypot(x, y);
  return std::atan2(nx, ny);
}

float64 AngleDiff(float64 a, float64 b) {
  return fmod(a - b + 3 * M_PI, 2 * M_PI) - M_PI;
}

void Main() {
  LadyBug& ladybug = LadyBug::Instance();
  std::atomic<bool> shutdown(false);
  float64 test = Angle(1, 2);
  LOGEXPR(test);
  std::thread t([&] {
    Sleep(200ms);
    auto state = ladybug.GetState();
    float64 start_angle = Angle(state.mx(), state.my());
    ladybug.SetLeftWheelPower(0.6);
    ladybug.SetRightWheelPower(0.6);

    bool forward = true;
    bool adjust = false;

    while (!shutdown) {
      state = ladybug.GetState();
      LOGEXPR(state.gz());
      LOGEXPR(state.left());
      LOGEXPR(state.right());
      float64 angle_diff =
          AngleDiff(start_angle, Angle(state.mx(), state.my()));
      LOGEXPR(angle_diff);

      if (state.sonar() != 0 && state.sonar() < 0.3) {
        forward = false;
        ladybug.SetLeftWheelPower(0.6);
        ladybug.SetRightWheelPower(-0.6);
      } else if (!forward) {
        start_angle = Angle(state.mx(), state.my());
        ladybug.SetLeftWheelPower(0);
        ladybug.SetRightWheelPower(0);
        forward = true;
        adjust = true;
      } else if (angle_diff > 0.1) {
        ladybug.SetLeftWheelPower(0);
        ladybug.SetRightWheelPower(0.7);
        adjust = true;
      } else if (angle_diff < -0.1) {
        ladybug.SetLeftWheelPower(0.7);
        ladybug.SetRightWheelPower(0);
        adjust = true;
      } else {
        if (adjust) {
          ladybug.SetLeftWheelPower(0.6);
          ladybug.SetRightWheelPower(0.6);
          state = ladybug.GetState();
          adjust = false;
        }
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
