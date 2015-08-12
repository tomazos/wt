#include "ladybug/accelerometer_subsystem.h"

constexpr int ACCELEROMETER_I2C_ADDRESS = 0x19;

constexpr int CTRL_REG1_A = 0x20;
constexpr int CTRL_REG2_A = 0x21;
constexpr int CTRL_REG3_A = 0x22;
constexpr int CTRL_REG4_A = 0x23;
constexpr int CTRL_REG5_A = 0x24;
constexpr int CTRL_REG6_A = 0x25;
constexpr int REFERENCE_A = 0x26;
constexpr int STATUS_REG_A = 0x27;

constexpr int OUT_X_L_A = 0x28;
constexpr int OUT_X_H_A = 0x29;
constexpr int OUT_Y_L_A = 0x2A;
constexpr int OUT_Y_H_A = 0x2B;
constexpr int OUT_Z_L_A = 0x2C;
constexpr int OUT_Z_H_A = 0x2D;

constexpr int FIFO_CTRL_REG_A = 0x2E;
constexpr int FIFO_SRC_REG_A = 0x2F;

AccelerometerSubsystem::AccelerometerSubsystem()
    : device(ACCELEROMETER_I2C_ADDRESS) {
  device.write_uint8(CTRL_REG1_A, 0b0101'011'1);
  device.write_uint8(CTRL_REG4_A, 0b1001'100'0);
  device.write_uint8(CTRL_REG5_A, 0b0100'000'0);
  device.write_uint8(FIFO_CTRL_REG_A, 0b0000'000'0);
  device.write_uint8(FIFO_CTRL_REG_A, 0b1000'000'0);
  Start();
}

AccelerometerSubsystem::~AccelerometerSubsystem() { Stop(); }

float64 AccelerometerSubsystem::ReadAccelerationReg(int OUT_H, int OUT_L) {
  int16 a = (device.read_uint8(OUT_L) | (device.read_uint8(OUT_H) << 8));
  return (float64(a) / float64(1 << 15)) * 4 /*gs*/;
}

void AccelerometerSubsystem::Tick() {
  uint8 fifo_src_reg_a = device.read_uint8(FIFO_SRC_REG_A);
  const int fss = fifo_src_reg_a & 0b11111;
  for (int i = 0; i < fss; ++i) {
    const float64 ax = ReadAccelerationReg(OUT_X_H_A, OUT_X_L_A);
    const float64 ay = ReadAccelerationReg(OUT_Y_H_A, OUT_Y_L_A);
    const float64 az = ReadAccelerationReg(OUT_Z_H_A, OUT_Z_L_A);
    {
      LockGuard l(mutex_);
      for (const Listener& listener : listeners_) {
        listener(ax, ay, az);
      }
    }
  }
  Sleep(5ms);
}
