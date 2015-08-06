#include "wiring/wiring.h"

#include <iostream>

#include "must.h"

constexpr int CTRL_REG1_A = 0x20;

constexpr int OUT_X_L_A = 0x28;
constexpr int OUT_X_H_A = 0x29;
constexpr int OUT_Y_L_A = 0x2A;
constexpr int OUT_Y_H_A = 0x2B;
constexpr int OUT_Z_L_A = 0x2C;
constexpr int OUT_Z_H_A = 0x2D;

constexpr int MR_REG_M = 0x02;

constexpr int OUT_X_H_M = 0x03;
constexpr int OUT_X_L_M = 0x04;
constexpr int OUT_Z_H_M = 0x05;
constexpr int OUT_Z_L_M = 0x06;
constexpr int OUT_Y_H_M = 0x07;
constexpr int OUT_Y_L_M = 0x08;

struct I2CDevice {
  I2CDevice(int device_id) : device_id_(device_id), fd_(wiringPiI2CSetup(device_id_)) {
    SetupWiringIfNeeded();
    MUST_NE(fd_,-1, "unable to open i2c device: ", device_id);
  }
  
  uint8 read(const int reg) {
    const int result = wiringPiI2CReadReg8(fd_, reg);
    MUST_NE(result, -1, "unable to read register ", reg, " from i2c device ", device_id_);
    return result & 0xFF;
  }
  
  void write(const int reg, const uint8 data) {
    int result = wiringPiI2CWriteReg8(fd_, reg, data);
    MUST_EQ(result, 0, "unable to write ", data, " to register ", reg, " on i2c device ", device_id_);
  }

 private:
  const int device_id_;
  const int fd_;
};

int main() {
  try {
    I2CDevice accelerometer(0x19);
    I2CDevice magnetometer(0x1e);
    accelerometer.write(CTRL_REG1_A, 0b01000111);
    magnetometer.write(MR_REG_M, 0b00000000);
    while (true) {
      int16 ax = (accelerometer.read(OUT_X_L_A) | (accelerometer.read(OUT_X_H_A) << 8));
      int16 ay = (accelerometer.read(OUT_Y_L_A) | (accelerometer.read(OUT_Y_H_A) << 8));
      int16 az = (accelerometer.read(OUT_Z_L_A) | (accelerometer.read(OUT_Z_H_A) << 8));

      int16 mx = (magnetometer.read(OUT_X_L_M) | (magnetometer.read(OUT_X_H_M) << 8));
      int16 my = (magnetometer.read(OUT_Y_L_M) | (magnetometer.read(OUT_Y_H_M) << 8));
      int16 mz = (magnetometer.read(OUT_Z_L_M) | (magnetometer.read(OUT_Z_H_M) << 8));
      
      std::cout << "ax = " << ax << ", ay = " << ay << ", az = " << az << ", ";
      std::cout << "mx = " << mx << ", my = " << my << ", mz = " << mz << std::endl;
      
      Sleep(1s);
    }
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    std::exit(1);
  }
}
