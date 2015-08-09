#include "ladybug/sonar_subsystem.h"

#include <iostream>

#include "wiring/wiring.h"

SonarSubsystem::SonarSubsystem(int trigger_pin, int echo_pin)
    : trigger_pin_(trigger_pin), echo_pin_(echo_pin) {
  SetupWiringIfNeeded();

  pinMode(trigger_pin_, OUTPUT);
  pinMode(echo_pin_, INPUT);

  digitalWrite(trigger_pin_, LOW);
  Start();
}

SonarSubsystem::~SonarSubsystem() { Stop(); }

void SonarSubsystem::AddListener(std::function<void(float64 meters)> listener) {
  LockGuard l(mutex_);
  listeners_.push_back(listener);
}

void SonarSubsystem::Tick() {
  Sleep(200ms);

  std::vector<Listener> listeners;
  {
    LockGuard l(mutex_);
    listeners = listeners_;
  }

  if (listeners.empty()) {
    return;
  }
  TriggerSonar();
  int64 microseconds = ListenSonarEchoMicros();
  float64 distance_meters = float64(microseconds) / 7000;
  for (auto listener : listeners) listener(distance_meters);
}

void SonarSubsystem::TriggerSonar() {
  std::cout << "triggering" << std::endl;
  digitalWrite(trigger_pin_, HIGH);
  Sleep(10us);
  digitalWrite(trigger_pin_, LOW);
}

int64 SonarSubsystem::ListenSonarEchoMicros() {
  std::cout << "waiting" << std::endl;

  auto start = Now();
  while (digitalRead(echo_pin_) == LOW) start = Now();

  auto end = Now();
  while (digitalRead(echo_pin_) == HIGH) end = Now();

  return (end - start) / 1us;
}
