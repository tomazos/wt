#include "wiring/i2c_device.h"

#include <iostream>

#include "core/must.h"

constexpr int CTRL_REG1_A = 0x20;

constexpr int OUT_X_L_A = 0x28;
constexpr int OUT_X_H_A = 0x29;
constexpr int OUT_Y_L_A = 0x2A;
constexpr int OUT_Y_H_A = 0x2B;
constexpr int OUT_Z_L_A = 0x2C;
constexpr int OUT_Z_H_A = 0x2D;

constexpr int CRA_REG_M = 0x00;
constexpr int MR_REG_M = 0x02;

constexpr int OUT_X_H_M = 0x03;
constexpr int OUT_X_L_M = 0x04;
constexpr int OUT_Z_H_M = 0x05;
constexpr int OUT_Z_L_M = 0x06;
constexpr int OUT_Y_H_M = 0x07;
constexpr int OUT_Y_L_M = 0x08;

constexpr int TEMP_OUT_H_M = 0x31;
constexpr int TEMP_OUT_L_M = 0x32;

constexpr int CTRL_REG1_G = 0x20;
constexpr int CTRL_REG2_G = 0x20;
constexpr int CTRL_REG3_G = 0x20;
constexpr int CTRL_REG4_G = 0x20;
constexpr int CTRL_REG5_G = 0x20;

constexpr int OUT_X_L_G = 0x28;
constexpr int OUT_X_H_G = 0x29;
constexpr int OUT_Y_L_G = 0x2A;
constexpr int OUT_Y_H_G = 0x2B;
constexpr int OUT_Z_L_G = 0x2C;
constexpr int OUT_Z_H_G = 0x2D;

constexpr int OUT_TEMP_G = 0x26;

int main() {
  try {
    I2CDevice accelerometer(0x19);
    I2CDevice gyroscope(0x6b);
    I2CDevice magnetometer(0x1e);
    accelerometer.write(CTRL_REG1_A, 0b0100'0111);
    magnetometer.write(CRA_REG_M, 0b1001'0000);
    magnetometer.write(MR_REG_M, 0b0000'0000);

    gyroscope.write(CTRL_REG1_G, 0b0110'1111);

    while (true) {
      int16 ax = (accelerometer.read(OUT_X_L_A) |
                  (accelerometer.read(OUT_X_H_A) << 8));
      int16 ay = (accelerometer.read(OUT_Y_L_A) |
                  (accelerometer.read(OUT_Y_H_A) << 8));
      int16 az = (accelerometer.read(OUT_Z_L_A) |
                  (accelerometer.read(OUT_Z_H_A) << 8));

      int16 gx = (gyroscope.read(OUT_X_L_G) | (gyroscope.read(OUT_X_H_G) << 8));
      int16 gy = (gyroscope.read(OUT_Y_L_G) | (gyroscope.read(OUT_Y_H_G) << 8));
      int16 gz = (gyroscope.read(OUT_Z_L_G) | (gyroscope.read(OUT_Z_H_G) << 8));

      int16 mx =
          (magnetometer.read(OUT_X_L_M) | (magnetometer.read(OUT_X_H_M) << 8));
      int16 my =
          (magnetometer.read(OUT_Y_L_M) | (magnetometer.read(OUT_Y_H_M) << 8));
      int16 mz =
          (magnetometer.read(OUT_Z_L_M) | (magnetometer.read(OUT_Z_H_M) << 8));

      int8 gtemp = gyroscope.read(OUT_TEMP_G);

      int16 mtemp = (magnetometer.read(TEMP_OUT_L_M) |
                     (magnetometer.read(TEMP_OUT_H_M) << 8));

      std::cout << "ax = " << ax << ", ay = " << ay << ", az = " << az << ", ";
      std::cout << "gx = " << gx << ", gy = " << gy << ", gz = " << gz << ", ";
      std::cout << "mx = " << mx << ", my = " << my << ", mz = " << mz << ", ";
      std::cout << "gtemp = " << int(gtemp) << ", mtemp = " << mtemp << ", ";
      std::cout << std::endl;

      Sleep(1s);
    }
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    std::exit(1);
  }
}
