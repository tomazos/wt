#include "ladybug/sonar_subsystem.h"

#include <iostream>

constexpr int kSonarTriggerPin = 29;
constexpr int kSonarEchoPin = 28;

int main() {
  try {
    SonarSubsystem sonar(kSonarTriggerPin, kSonarEchoPin);
    sonar.AddListener([](float64 meters) {
      std::cout << meters * 1000 << "mm" << std::endl;
    });

    std::getchar();
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    std::exit(1);
  }
}
