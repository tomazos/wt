#include "wiring/i2c_device.h"

#include <iostream>

#include "core/must.h"

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

inline uint16 swapends(uint16 x) {
  const int a = x & 0xFF;
  const int b = (x >> 8) & 0xFF;
  return (a << 8) | b;
}

int main() {
  try {
    // I2CDevice accelerometer(0x19);
    I2CDevice gyroscope(0x6b);
    //    I2CDevice magnetometer(0x1e);
    //    accelerometer.write_uint8(CTRL_REG1_A, 0b0101'011'1);
    //    accelerometer.write_uint8(CTRL_REG4_A, 0b1000'000'0);
    //    accelerometer.write_uint8(CTRL_REG5_A, 0b0100'000'0);
    //    accelerometer.write_uint8(FIFO_CTRL_REG_A, 0b0000'000'0);
    //    accelerometer.write_uint8(FIFO_CTRL_REG_A, 0b1000'000'0);

    //        magnetometer.write(CRA_REG_M, 0b1001'0000);
    //        magnetometer.write(MR_REG_M, 0b0000'0000);

    //    gyroscope.write(CTRL_REG1_G, 0b0110'1111);

    auto start = Now();
    std::cout << decltype(start)::period::num << "/"
              << decltype(start)::period::den << std::endl;

    int64 samples = 0;
    int64 loops = 0;
    while (true) {
      loops++;

      if (!(loops & (loops - 1))) {
        auto duration = Now() - start;
        float64 duration_secs = float64(duration / 1ns) / 1'000'000'000;
        std::cout << duration_secs << " sample = " << (samples / duration_secs)
                  << "Hz (" << samples << ")"
                  << ", loop = " << (loops / duration_secs) << "Hz (" << loops
                  << ")" << std::endl;
      }

      // uint8 status_a = accelerometer.read_uint8(STATUS_REG_A);
      volatile uint8 fifo_src_reg_a[[gnu::unused]] =
          accelerometer.read_uint8(FIFO_SRC_REG_A);
      //      int fss = fifo_src_reg_a & 0b11111;
      //      int zyxda = bool(status_a & (1 << 3));
      //      std::cout << "ZYXDA = " << zyxda << ", FSS = " << fss <<
      //      std::endl;

      //      if (fss == 0) {
      //        //        Sleep(1ms);
      //      }
      //      for (int i = 0; i < fss; i++) {
      //        int16 ax[[gnu::unused]] = (accelerometer.read_uint8(OUT_X_L_A) |
      //                                   (accelerometer.read_uint8(OUT_X_H_A)
      //                                   << 8));
      //        int16 ay[[gnu::unused]] = (accelerometer.read_uint8(OUT_Y_L_A) |
      //                                   (accelerometer.read_uint8(OUT_Y_H_A)
      //                                   << 8));
      //        int16 az[[gnu::unused]] = (accelerometer.read_uint8(OUT_Z_L_A) |
      //                                   (accelerometer.read_uint8(OUT_Z_H_A)
      //                                   << 8));

      //        samples++;
      //      }
    }

    //      int16 gx = swapends(accelerometer.read_uint16(OUT_X_L_A));
    //      int16 gy = swapends(accelerometer.read_uint16(OUT_Y_L_A));
    //      int16 gz = swapends(accelerometer.read_uint16(OUT_Z_L_A));

    //      int16 gx = (gyroscope.read_uint8(OUT_X_L_G) |
    //      (gyroscope.read_uint8(OUT_X_H_G) << 8));
    //      int16 gy = (gyroscope.read_uint8(OUT_Y_L_G) |
    //      (gyroscope.read_uint8(OUT_Y_H_G) << 8));
    //      int16 gz = (gyroscope.read_uint8(OUT_Z_L_G) |
    //      (gyroscope.read_uint8(OUT_Z_H_G) << 8));

    //      int16 mx =
    //          (magnetometer.read_uint8(OUT_X_L_M) |
    //          (magnetometer.read_uint8(OUT_X_H_M) << 8));
    //      int16 my =
    //          (magnetometer.read_uint8(OUT_Y_L_M) |
    //          (magnetometer.read_uint8(OUT_Y_H_M) << 8));
    //      int16 mz =
    //          (magnetometer.read_uint8(OUT_Z_L_M) |
    //          (magnetometer.read_uint8(OUT_Z_H_M) << 8));

    //      int8 gtemp = gyroscope.read_uint8(OUT_TEMP_G);

    //      int16 mtemp = (magnetometer.read_uint8(TEMP_OUT_L_M) |
    //                     (magnetometer.read_uint8(TEMP_OUT_H_M) << 8));

    //            std::cout << "gx = " << gx << ", gy = " << gy << ", gz = "
    //            << gz
    //            << ", ";
    //      std::cout << "mx = " << mx << ", my = " << my << ", mz = " << mz
    //      << ", ";
    //      std::cout << "gtemp = " << int(gtemp) << ", mtemp = " << mtemp <<
    //      ", ";
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    std::exit(1);
  }
}
