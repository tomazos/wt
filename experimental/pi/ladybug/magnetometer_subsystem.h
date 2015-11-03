#pragma once

#include "ladybug/subsystem.h"
#include "wiring/i2c_device.h"

class MagnetometerSubsystem : public Subsystem {
 public:
  MagnetometerSubsystem();
  ~MagnetometerSubsystem();

  using Listener = std::function<void(float32 x, float32 y, float32 z)>;
  void AddListener(Listener listener) {
    LockGuard l(mutex_);
    listeners_.push_back(listener);
  }

 protected:
  void Tick() override;

 private:
  float64 ReadMagneticFieldReg(int OUT_H, int OUT_L);

  Mutex mutex_;
  std::vector<Listener> listeners_;
  I2CDevice device;
};
