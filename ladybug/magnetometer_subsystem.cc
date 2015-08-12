#include "ladybug/magnetometer_subsystem.h"

constexpr int MAGNETOMETER_I2C_ADDRESS = 0x1E;

constexpr int CRA_REG_M = 0x00;
constexpr int CRB_REG_M = 0x00;
constexpr int MR_REG_M = 0x02;

constexpr int OUT_X_H_M = 0x03;
constexpr int OUT_X_L_M = 0x04;
constexpr int OUT_Z_H_M = 0x05;
constexpr int OUT_Z_L_M = 0x06;
constexpr int OUT_Y_H_M = 0x07;
constexpr int OUT_Y_L_M = 0x08;

constexpr int TEMP_OUT_H_M = 0x31;
constexpr int TEMP_OUT_L_M = 0x32;

MagnetometerSubsystem::MagnetometerSubsystem()
    : device(MAGNETOMETER_I2C_ADDRESS) {
  device.write_uint8(CRA_REG_M, 0b0001'1100);
  device.write_uint8(CRB_REG_M, 0b1000'0000);
  device.write_uint8(MR_REG_M, 0b0000'0000);

  Start();
}

MagnetometerSubsystem::~MagnetometerSubsystem() { Stop(); }

float64 MagnetometerSubsystem::ReadMagneticFieldReg(int OUT_H, int OUT_L) {
  int16 m = ((device.read_uint8(OUT_H) << 8) | device.read_uint8(OUT_L));
  return m;
}

void MagnetometerSubsystem::Tick() {
  const float64 mx = ReadMagneticFieldReg(OUT_X_H_M, OUT_X_L_M);
  const float64 mz = ReadMagneticFieldReg(OUT_Z_H_M, OUT_Z_L_M);
  const float64 my = ReadMagneticFieldReg(OUT_Y_H_M, OUT_Y_L_M);
  {
    LockGuard l(mutex_);
    for (const Listener& listener : listeners_) {
      listener(mx, my, mz);
    }
  }
  Sleep(5ms);
}
