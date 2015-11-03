#include "wiring/i2c_device.h"

#include "core/must.h"
#include "wiring/wiring.h"

I2CDevice::I2CDevice(int device_id)
    : device_id_(device_id), fd_(wiringPiI2CSetup(device_id_)) {
  SetupWiringIfNeeded();
  MUST_NE(fd_, -1, "unable to open i2c device: ", device_id);
}
