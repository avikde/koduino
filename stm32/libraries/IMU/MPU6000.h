/**
 * @authors Avik De <avikde@gmail.com>

  This file is part of koduino <https://github.com/avikde/koduino>

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation, either
  version 3 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, see <http://www.gnu.org/licenses/>.
 */
#ifndef MPU6000_h
#define MPU6000_h

#include "IMU.h"
#include <SPI.h>

/** @addtogroup IMU
 *  @{
 */

// MPU 6000 registers
#define MPUREG_WHOAMI     0x75
#define MPUREG_SMPLRT_DIV   0x19
#define MPUREG_CONFIG     0x1A
#define MPUREG_GYRO_CONFIG    0x1B
#define MPUREG_ACCEL_CONFIG   0x1C
#define MPUREG_FIFO_EN      0x23
#define MPUREG_INT_PIN_CFG    0x37
#define MPUREG_INT_ENABLE   0x38
#define MPUREG_INT_STATUS   0x3A
#define MPUREG_ACCEL_XOUT_H   0x3B
#define MPUREG_ACCEL_XOUT_L   0x3C
#define MPUREG_ACCEL_YOUT_H   0x3D
#define MPUREG_ACCEL_YOUT_L   0x3E
#define MPUREG_ACCEL_ZOUT_H   0x3F
#define MPUREG_ACCEL_ZOUT_L   0x40
#define MPUREG_TEMP_OUT_H   0x41
#define MPUREG_TEMP_OUT_L   0x42
#define MPUREG_GYRO_XOUT_H    0x43
#define MPUREG_GYRO_XOUT_L    0x44
#define MPUREG_GYRO_YOUT_H    0x45
#define MPUREG_GYRO_YOUT_L    0x46
#define MPUREG_GYRO_ZOUT_H    0x47
#define MPUREG_GYRO_ZOUT_L    0x48
#define MPUREG_USER_CTRL    0x6A
#define MPUREG_PWR_MGMT_1   0x6B
#define MPUREG_PWR_MGMT_2   0x6C
#define MPUREG_FIFO_COUNTH    0x72
#define MPUREG_FIFO_COUNTL    0x73
#define MPUREG_FIFO_R_W     0x74


// Configuration bits
#define BIT_SLEEP       0x40
#define BIT_H_RESET       0x80
#define BITS_CLKSEL       0x07
#define MPU_CLK_SEL_PLLGYROX  0x01
#define MPU_CLK_SEL_PLLGYROZ  0x03
#define MPU_EXT_SYNC_GYROX    0x02
#define BITS_FS_250DPS          0x00
#define BITS_FS_500DPS          0x08
#define BITS_FS_1000DPS         0x10
#define BITS_FS_2000DPS         0x18
#define BITS_FS_MASK            0x18
#define BITS_DLPF_CFG_256HZ_NOLPF2  0x00
#define BITS_DLPF_CFG_188HZ         0x01
#define BITS_DLPF_CFG_98HZ          0x02
#define BITS_DLPF_CFG_42HZ          0x03
#define BITS_DLPF_CFG_20HZ          0x04
#define BITS_DLPF_CFG_10HZ          0x05
#define BITS_DLPF_CFG_5HZ           0x06
#define BITS_DLPF_CFG_2100HZ_NOLPF  0x07
#define BITS_DLPF_CFG_MASK          0x07
#define BIT_INT_ANYRD_2CLEAR    0x10
#define BIT_RAW_RDY_EN      0x01
#define BIT_I2C_IF_DIS          0x10
#define BIT_INT_STATUS_DATA   0x01


typedef struct {
  short x;
  short y;
  short z;
} tAxis;

union MPUData {
  unsigned char rawByte[];
  unsigned short rawWord[];
  struct {
    tAxis accel;
    short temperature;
    tAxis gyro;
  } data;
};

// cldoc:begin-category(IMU)

class MPU6000 : public IMU {
public:
  // constructors set up which SPI
  MPU6000(SPIClass& theSPI) : theSPI(theSPI) {}
  MPU6000() : theSPI(SPI) {}

  bool init(uint8_t csPin);
  void read(byte reg, unsigned int length, byte *data);
  byte read(byte reg);
  void write(byte reg, unsigned int length, const byte *data);
  void write(byte reg, byte data);
  void swapData(unsigned char *data, int datalen);
  virtual void readSensors();

// protected:
  MPUData data;
  uint8_t csPin;
  unsigned long csDelay;
  SPIClass& theSPI;
};

/** @} */ // end of addtogroup

#endif