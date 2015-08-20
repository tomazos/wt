#pragma once

#include <memory>

#include "ladybug/accelerometer_subsystem.h"
#include "ladybug/camera_subsystem.h"
#include "ladybug/gyroscope_subsystem.h"
#include "ladybug/magnetometer_subsystem.h"
#include "ladybug/motor_subsystem.h"
#include "ladybug/ladybug_state.pb.h"
#include "ladybug/sonar_subsystem.h"

class LadyBug {
 public:
  static LadyBug& Instance() {
    static LadyBug ladybug;
    return ladybug;
  }

  using State = LadyBugState;

  // -1.0 .. +1.0
  void SetLeftWheelPower(float64 power) {
    left_wheel_.SetPower(power);
    LockGuard l(mutex_);
    state_.set_left(power);
  }

  // -1.0 .. +1.0
  void SetRightWheelPower(float64 power) {
    right_wheel_.SetPower(power);
    LockGuard l(mutex_);
    state_.set_right(power);
  }

  State GetState() {
    State result;
    {
      LockGuard l(mutex_);
      result = state_;
    }
    return result;
  }

 private:
  LadyBug();

  Mutex mutex_;
  State state_;

  MotorSubsystem left_wheel_, right_wheel_;
  SonarSubsystem sonar_;
  AccelerometerSubsystem accelerometer_;
  GyroscopeSubsystem gyroscope_;
  MagnetometerSubsystem magnetometer_;
  CameraSubsystem camera_;

  LadyBug(const LadyBug&) = delete;
  LadyBug(LadyBug&&) = delete;
  void operator=(const LadyBug&) = delete;
  void operator=(LadyBug&&) = delete;
};
