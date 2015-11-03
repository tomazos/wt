#pragma once

#include "ladybug/subsystem.h"

class MotorSubsystem : public Subsystem {
 public:
  MotorSubsystem(int enable_pin, int direction1_pin, int direction2_pin);
  ~MotorSubsystem();

  void SetPower(float64 power);

 protected:
  void Tick() override;

 private:
  enum Direction { FORWARD, BACKWARD, NEUTRAL };

  void SetEnable(bool enable);

  void SetDirection(Direction direction);

  const int enable_pin_, direction1_pin_, direction2_pin_;

  Mutex mutex_;
  float64 requested_power_, power_;
};
