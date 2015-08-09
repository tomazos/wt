#include "ladybug/ladybug.h"

constexpr int kLeftMotorEnablePin = 0;
constexpr int kLeftMotorDirectionPin1 = 2;
constexpr int kLeftMotorDirectionPin2 = 4;

constexpr int kRightMotorEnablePin = 1;
constexpr int kRightMotorDirectionPin1 = 3;
constexpr int kRightMotorDirectionPin2 = 5;

constexpr int kSonarTriggerPin = 29;
constexpr int kSonarEchoPin = 28;

LadyBug::LadyBug()
    : left_wheel_(kLeftMotorEnablePin, kLeftMotorDirectionPin1,
                  kLeftMotorDirectionPin2),
      right_wheel_(kRightMotorEnablePin, kRightMotorDirectionPin1,
                   kRightMotorDirectionPin2),
      sonar_(kSonarTriggerPin, kSonarEchoPin) {}
