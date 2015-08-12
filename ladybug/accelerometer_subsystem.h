#pragma once

#include "ladybug/subsystem.h"
#include "wiring/i2c_device.h"

class AccelerometerSubsystem : public Subsystem {
 public:
  AccelerometerSubsystem();
  ~AccelerometerSubsystem();

  using Listener = std::function<void(float64 x, float64 y, float64 z)>;
  void AddListener(Listener listener) {
    LockGuard l(mutex_);
    listeners_.push_back(listener);
  }

 protected:
  void Tick() override;

 private:
  float64 ReadAccelerationReg(int OUT_H, int OUT_L);

  Mutex mutex_;
  std::vector<Listener> listeners_;
  I2CDevice device;
};
