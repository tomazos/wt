#pragma once

#include "core/must.h"
#include "wiring/wiring.h"

class I2CDevice {
 public:
  I2CDevice(int device_id);

  inline uint8 read_uint8(const int reg);
  inline uint16 read_uint16(const int reg);
  int8 read_int8(const int reg) { return read_uint8(reg); }
  int16 read_int16(const int reg) { return read_uint16(reg); }

  inline void write_uint8(const int reg, const uint8 data);
  inline void write_uint16(const int reg, const uint16 data);
  void write_int8(const int reg, const int8 data) { write_uint8(reg, data); }
  void write_int16(const int reg, const int16 data) { write_uint16(reg, data); }

 private:
  const int device_id_;
  const int fd_;
};

inline uint8 I2CDevice::read_uint8(const int reg) {
  const int result = wiringPiI2CReadReg8(fd_, reg);
  MUST_NE(result, -1, "unable to read register ", reg, " from i2c device ",
          device_id_);
  return result & 0xFF;
}

inline uint16 I2CDevice::read_uint16(const int reg) {
  const int result = wiringPiI2CReadReg16(fd_, reg);
  MUST_NE(result, -1, "unable to read register ", reg, " from i2c device ",
          device_id_);
  return result & 0xFFFF;
}

inline void I2CDevice::write_uint8(const int reg, const uint8 data) {
  int result = wiringPiI2CWriteReg8(fd_, reg, data);
  MUST_EQ(result, 0, "unable to write ", data, " to register ", reg,
          " on i2c device ", device_id_);
}

inline void I2CDevice::write_uint16(const int reg, const uint16 data) {
  int result = wiringPiI2CWriteReg16(fd_, reg, data);
  MUST_EQ(result, 0, "unable to write ", data, " to register ", reg,
          " on i2c device ", device_id_);
}
