#include "wiring/i2c_device.h"

#include "core/must.h"
#include "wiring/wiring.h"

I2CDevice::I2CDevice(int device_id)
    : device_id_(device_id), fd_(wiringPiI2CSetup(device_id_)) {
  SetupWiringIfNeeded();
  MUST_NE(fd_, -1, "unable to open i2c device: ", device_id);
}

uint8 I2CDevice::read(const int reg) {
  const int result = wiringPiI2CReadReg8(fd_, reg);
  MUST_NE(result, -1, "unable to read register ", reg, " from i2c device ",
          device_id_);
  return result & 0xFF;
}

void I2CDevice::write(const int reg, const uint8 data) {
  int result = wiringPiI2CWriteReg8(fd_, reg, data);
  MUST_EQ(result, 0, "unable to write ", data, " to register ", reg,
          " on i2c device ", device_id_);
}
