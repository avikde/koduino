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
#include "MPU6000.h"

bool MPU6000::init(uint8_t csPin) {
  // Should any of these be set by the user
  this->csDelay = 1;
  this->csPin = csPin;

  // init SPI
  pinMode(csPin, OUTPUT);
  digitalWrite(csPin, HIGH);
  theSPI.begin();
  theSPI.setClockDivider(SPI_CLOCK_DIV2);
  theSPI.setBitOrder(MSBFIRST);
  theSPI.setDataMode(SPI_MODE0);

  // Start communicating
  byte result = read(MPUREG_WHOAMI);
  if ((result & 0x7E) != 0x68)
    return false;

  // Chip reset
  write(MPUREG_PWR_MGMT_1, BIT_H_RESET);
  delay(100);  // Startup time delay

  // Disable I2C bus
  write(MPUREG_USER_CTRL, BIT_I2C_IF_DIS);

  // Wake Up device and select GyroZ clock
  write(MPUREG_PWR_MGMT_1, MPU_CLK_SEL_PLLGYROZ);
  write(MPUREG_PWR_MGMT_2, 0);

  // SAMPLE RATE
  write(MPUREG_SMPLRT_DIV, 0x00);    // Sample rate = 1kHz

  // FS & DLPF   FS=1000deg/s, DLPF = 42Hz (low pass filter)
  write(MPUREG_CONFIG, BITS_DLPF_CFG_42HZ);
  write(MPUREG_GYRO_CONFIG, BITS_FS_1000DPS); // Gyro scale 1000deg/s
  write(MPUREG_ACCEL_CONFIG, 0x03<<3);  // Accel scale +-16g
  return true;
}

void MPU6000::read(byte reg, unsigned int length, byte *data) {
  digitalWrite(csPin, LOW);
  delayMicroseconds(csDelay);
  theSPI.transfer(reg | 0x80);
  for (int i = 0; i < length; ++i) {
    data[i] = theSPI.transfer(0);
  }
  digitalWrite(csPin, HIGH);
}

byte MPU6000::read(byte reg) {
  byte res = 0;
  read(reg, 1, &res);
  return res;
}

void MPU6000::write(byte reg, unsigned int length, const byte *data) {
  digitalWrite(csPin, LOW);
  delayMicroseconds(csDelay);
  theSPI.transfer(reg);
  for (int i = 0; i < length; ++i) {
    theSPI.transfer(data[i]);
  }
  digitalWrite(csPin, HIGH);
}

void MPU6000::write(byte reg, byte data) {
  write(reg, 1, &data);
}

void MPU6000::swapData(unsigned char *data, int datalen) {
  datalen /= 2;
  while (datalen--) {
    unsigned char t = data[0];
    data[0] = data[1];
    data[1] = t;
    data += 2;
  }
}

void MPU6000::readSensors() {
  const float accSens = 9.8 / 2048.0;
  const float gyrSens = PI / (32.8*180);
  read(MPUREG_ACCEL_XOUT_H, sizeof(data), data.rawByte);
  swapData(data.rawByte, sizeof(data));
  acc[0] = accSens * data.data.accel.x;
  acc[1] = accSens * data.data.accel.y;
  acc[2] = accSens * data.data.accel.z;
  gyr[0] = gyrSens * data.data.gyro.x;
  gyr[1] = gyrSens * data.data.gyro.y;
  gyr[2] = gyrSens * data.data.gyro.z;
}
