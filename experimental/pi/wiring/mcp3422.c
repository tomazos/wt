/*
 * mcp3422.c:
 *	Extend wiringPi with the MCP3422 I2C ADC chip
 *	Also works for the MCP3423 and MCP3224 (4 channel) chips
 *	Copyright (c) 2013 Gordon Henderson
 ***********************************************************************
 * This file is part of wiringPi:
 *	https://projects.drogon.net/raspberry-pi/wiringpi/
 *
 *    wiringPi is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU Lesser General Public License as
 *    published by the Free Software Foundation, either version 3 of the
 *    License, or (at your option) any later version.
 *
 *    wiringPi is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public
 *    License along with wiringPi.
 *    If not, see <http://www.gnu.org/licenses/>.
 ***********************************************************************
 */

#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>

#include "core/must.h"

#include "wiring/wiringPi.h"
#include "wiring/wiringPiI2C.h"

#include "wiring/mcp3422.h"

/*
 * myAnalogRead:
 *	Read a channel from the device
 *********************************************************************************
 */

int myAnalogRead(struct wiringPiNodeStruct *node, int chan) {
  unsigned char config;
  unsigned char buffer[4];
  int value = 0;

  // One-shot mode, trigger plus the other configs.

  config =
      0x80 | ((chan - node->pinBase) << 5) | (node->data0 << 2) | (node->data1);

  wiringPiI2CWrite(node->fd, config);

  switch (node->data0)  // Sample rate
  {
    case MCP3422_SR_3_75:  // 18 bits
      delay(270);
      MUST_EQ(4, read(node->fd, buffer, 4));
      value = ((buffer[0] & 3) << 16) | (buffer[1] << 8) | buffer[0];
      break;

    case MCP3422_SR_15:  // 16 bits
      delay(70);
      MUST_EQ(3, read(node->fd, buffer, 3));
      value = (buffer[0] << 8) | buffer[1];
      break;

    case MCP3422_SR_60:  // 14 bits
      delay(17);
      MUST_EQ(3, read(node->fd, buffer, 3));
      value = ((buffer[0] & 0x3F) << 8) | buffer[1];
      break;

    case MCP3422_SR_240:  // 12 bits
      delay(5);
      MUST_EQ(3, read(node->fd, buffer, 3));
      value = ((buffer[0] & 0x0F) << 8) | buffer[0];
      break;
  }

  return value;
}

/*
 * mcp3422Setup:
 *	Create a new wiringPi device node for the mcp3422
 *********************************************************************************
 */

int mcp3422Setup(int pinBase, int i2cAddress, int sampleRate, int gain) {
  int fd;
  struct wiringPiNodeStruct *node;

  if ((fd = wiringPiI2CSetup(i2cAddress)) < 0) return fd;

  node = wiringPiNewNode(pinBase, 4);

  node->data0 = sampleRate;
  node->data1 = gain;
  node->analogRead = myAnalogRead;

  return 0;
}
