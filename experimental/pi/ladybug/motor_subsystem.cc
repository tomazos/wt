#include "ladybug/motor_subsystem.h"

#include "core/must.h"
#include "core/random.h"
#include "wiring/wiring.h"

MotorSubsystem::MotorSubsystem(int enable_pin, int direction1_pin,
                               int direction2_pin)
    : enable_pin_(enable_pin),
      direction1_pin_(direction1_pin),
      direction2_pin_(direction2_pin),
      requested_power_(0),
      power_(0) {
  SetupWiringIfNeeded();

  pinMode(enable_pin_, OUTPUT);
  pinMode(direction1_pin_, OUTPUT);
  pinMode(direction2_pin_, OUTPUT);

  SetEnable(false);
  SetDirection(NEUTRAL);
  Start();
}

void MotorSubsystem::SetPower(const float64 power) {
  LockGuard l(mutex_);

  requested_power_ = power;
}

void MotorSubsystem::SetEnable(bool enable) {
  digitalWrite(enable_pin_, enable ? LOW : HIGH);
}

void MotorSubsystem::SetDirection(const Direction direction) {
  switch (direction) {
    case FORWARD:
      digitalWrite(direction1_pin_, HIGH);
      digitalWrite(direction2_pin_, LOW);
      break;
    case BACKWARD:
      digitalWrite(direction1_pin_, LOW);
      digitalWrite(direction2_pin_, HIGH);
      break;
    case NEUTRAL:
      digitalWrite(direction1_pin_, HIGH);
      digitalWrite(direction2_pin_, HIGH);
      break;
    default:
      FAIL();
  }
}

void MotorSubsystem::Tick() {
  {
    LockGuard l(mutex_);
    if (power_ != requested_power_) {
      power_ = requested_power_;

      if (power_ > 0) {
        SetDirection(FORWARD);
      } else if (power_ == 0) {
        SetDirection(NEUTRAL);
      } else {
        MUST(power_ < 0);
        SetDirection(BACKWARD);
      }
    }
  }

  constexpr int32 tick_duration_microseconds = 10000;
  int32 enable_duration_microseconds =
      std::lround(tick_duration_microseconds * std::abs(power_));
  MUST_LE(enable_duration_microseconds, tick_duration_microseconds);
  MUST_GE(enable_duration_microseconds, 0);
  int32 disable_duration_microseconds =
      tick_duration_microseconds - enable_duration_microseconds;
  if (enable_duration_microseconds > 0) {
    SetEnable(true);
    Sleep(std::chrono::microseconds(enable_duration_microseconds));
  }
  if (disable_duration_microseconds > 0) {
    SetEnable(false);
    Sleep(std::chrono::microseconds(disable_duration_microseconds));
  }
  SetEnable(RandFloat() < std::abs(power_));
}

MotorSubsystem::~MotorSubsystem() {
  Stop();
  SetEnable(false);
  SetDirection(NEUTRAL);
}
