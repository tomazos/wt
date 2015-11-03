#include "ladybug/gyroscope_subsystem.h"

constexpr int GYROSCOPE_I2C_ADDRESS = 0x6B;

constexpr int CTRL_REG1_G = 0x20;
constexpr int CTRL_REG2_G = 0x21;
constexpr int CTRL_REG3_G = 0x22;
constexpr int CTRL_REG4_G = 0x23;
constexpr int CTRL_REG5_G = 0x24;

constexpr int OUT_X_L_G = 0x28;
constexpr int OUT_X_H_G = 0x29;
constexpr int OUT_Y_L_G = 0x2A;
constexpr int OUT_Y_H_G = 0x2B;
constexpr int OUT_Z_L_G = 0x2C;
constexpr int OUT_Z_H_G = 0x2D;

constexpr int FIFO_CTRL_G = 0x2E;
constexpr int FIFO_SRC_G = 0x2F;

// http://www.st.com/st-web-ui/static/active/en/resource/technical/document/datasheet/DM00060659.pdf
GyroscopeSubsystem::GyroscopeSubsystem() : device(GYROSCOPE_I2C_ADDRESS) {
  device.write_uint8(CTRL_REG1_G, 0b0001'1111);
  device.write_uint8(CTRL_REG4_G, 0b0001'0000);
  device.write_uint8(CTRL_REG5_G, 0b0100'0000);
  device.write_uint8(FIFO_CTRL_G, 0b0000'000'0);
  device.write_uint8(FIFO_CTRL_G, 0b0100'0000);
  Start();
}

GyroscopeSubsystem::~GyroscopeSubsystem() { Stop(); }

float64 GyroscopeSubsystem::ReadAngularRateReg(int OUT_H, int OUT_L) {
  int16 g = (device.read_uint8(OUT_L) | (device.read_uint8(OUT_H) << 8));
  return (float64(g) / float64(1 << 15)) * 2 * M_PI * 500.0 / 360.0;
}

void GyroscopeSubsystem::Tick() {
  uint8 fifo_src_g = device.read_uint8(FIFO_SRC_G);
  const int fss = fifo_src_g & 0b11111;
  for (int i = 0; i < fss; ++i) {
    const float64 gx = ReadAngularRateReg(OUT_X_H_G, OUT_X_L_G);
    const float64 gy = ReadAngularRateReg(OUT_Y_H_G, OUT_Y_L_G);
    const float64 gz = ReadAngularRateReg(OUT_Z_H_G, OUT_Z_L_G);
    {
      LockGuard l(mutex_);
      for (const Listener& listener : listeners_) {
        listener(gx, gy, gz);
      }
    }
  }
  Sleep(100ms);
}
