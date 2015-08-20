#include "ladybug/ladybug.h"

constexpr int kLeftMotorEnablePin = 0;
constexpr int kLeftMotorDirectionPin1 = 2;
constexpr int kLeftMotorDirectionPin2 = 4;

constexpr int kRightMotorEnablePin = 1;
constexpr int kRightMotorDirectionPin1 = 3;
constexpr int kRightMotorDirectionPin2 = 5;

constexpr int kSonarTriggerPin = 29;
constexpr int kSonarEchoPin = 28;

static inline float64 Angle(float64 x, float64 y) {
  float64 nx = x / std::hypot(x, y);
  float64 ny = y / std::hypot(x, y);
  return std::atan2(nx, ny);
}

LadyBug::LadyBug()
    : left_wheel_(kLeftMotorEnablePin, kLeftMotorDirectionPin1,
                  kLeftMotorDirectionPin2),
      right_wheel_(kRightMotorEnablePin, kRightMotorDirectionPin1,
                   kRightMotorDirectionPin2),
      sonar_(kSonarTriggerPin, kSonarEchoPin) {
  sonar_.AddListener([this](float64 meters) {
    LockGuard l(mutex_);
    state_.set_sonar(meters);
  });
  accelerometer_.AddListener([this](float64 ax, float64 ay, float64 az) {
    LockGuard l(mutex_);
    state_.set_ax(ax);
    state_.set_ay(ay);
    state_.set_az(az);
  });
  gyroscope_.AddListener([this](float64 gx, float64 gy, float64 gz) {
    LockGuard l(mutex_);
    state_.set_gx(gx);
    state_.set_gy(gy);
    state_.set_gz(gz);

    float64 d = state_.direction2();
    d -= gz / GyroscopeSubsystem::kFrequencyHertz;
    while (d > M_PI) d -= 2 * M_PI;
    while (d < -M_PI) d += 2 * M_PI;

    state_.set_direction2(d);
  });
  magnetometer_.AddListener([this](float64 mx, float64 my, float64 mz) {
    LockGuard l(mutex_);
    state_.set_mx(mx);
    state_.set_my(my);
    state_.set_mz(mz);
    state_.set_direction1(-Angle(mx, my));
  });
  camera_.AddListener([this](uint8* data) {
    LockGuard l(mutex_);
    state_.set_image(data, CameraSubsystem::kBytes);
  });
}
