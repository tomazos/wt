#pragma once

#include <memory>
#include <mutex>

#include "ladybug/motor_subsystem.h"
#include "ladybug/sonar_subsystem.h"

class LadyBug {
 public:
  LadyBug();

  // -1.0 .. +1.0
  void SetLeftWheelPower(float64 power) { left_wheel_.SetPower(power); }

  // -1.0 .. +1.0
  void SetRightWheelPower(float64 power) { right_wheel_.SetPower(power); }

  void AddSonarListener(std::function<void(float64 meters)> listener) {
    sonar_.AddListener(listener);
  }

 private:
  MotorSubsystem left_wheel_, right_wheel_;
  SonarSubsystem sonar_;
};
