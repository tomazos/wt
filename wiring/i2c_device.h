#pragma once

#include "wiring/wiring.h"

class I2CDevice {
 public:
  I2CDevice(int device_id);

  uint8 read(const int reg);

  void write(const int reg, const uint8 data);

 private:
  const int device_id_;
  const int fd_;
};
