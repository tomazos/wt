#pragma once

#include <vector>

#include "ladybug/subsystem.h"

class SonarSubsystem : public Subsystem {
 public:
  SonarSubsystem(int trigger_pin, int echo_pin);
  ~SonarSubsystem();
  
  using Listener = std::function<void(float64 meters)>;
  void AddListener(Listener listener);

 protected:
  void Tick() override;

 private:
  void TriggerSonar();
  int64 ListenSonarEchoMicros();

  const int trigger_pin_, echo_pin_;

  Mutex mutex_;
  std::vector<Listener> listeners_;
};
