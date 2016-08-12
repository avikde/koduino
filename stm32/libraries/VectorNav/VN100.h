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
#ifndef VN100_h
#define VN100_h

#include <SPI.h>


/**
 * @brief VN100 hardware interface library
 */
class VN100 {
public:
  SPIClass& theSPI;
  uint8_t csPin;

  /**
   * @brief Construct with reference to which SPI
   * @param theSPI SPI, SPI_2, etc.
   */
  VN100(SPIClass& theSPI) : theSPI(theSPI) {}
  
  /**
   * @brief Send initialization commands to the VN100
   * 
   * @param csPin chip select pin
   */
  void init(uint8_t csPin) {
    this->csPin = csPin;

    pinMode(csPin, OUTPUT);
    digitalWrite(csPin, HIGH);

    theSPI.begin();
    theSPI.setClockDivider(SPI_CLOCK_DIV2);
    theSPI.setBitOrder(MSBFIRST);
    theSPI.setDataMode(SPI_MODE3);
  }

  void readReg(uint8_t reg, int N, uint8_t *buf) {
    digitalWrite(csPin, LOW);
    delayMicroseconds(1);
    theSPI.transfer(0x01);
    theSPI.transfer(reg);
    theSPI.transfer(0x00);
    theSPI.transfer(0x00);
    delayMicroseconds(1);
    digitalWrite(csPin, HIGH);
    delayMicroseconds(100);

    digitalWrite(csPin, LOW);
    delayMicroseconds(1);
    for (int i=0; i<N+4; ++i) {
      uint8_t c = theSPI.transfer(0x00);
      // if (i==3 && c!= 0)
      if (i >= 4) {
        buf[i-4] = c;
      }
    }
    delayMicroseconds(1);
    digitalWrite(csPin, HIGH);
  }
  void writeReg(uint8_t reg, int N, const uint8_t *args) {
    digitalWrite(csPin, LOW);
    delayMicroseconds(1);
    theSPI.transfer(0x01);
    theSPI.transfer(reg);
    theSPI.transfer(0x00);
    theSPI.transfer(0x00);
    for (int i=0; i<N; ++i)
      theSPI.transfer(args[i]);
    delayMicroseconds(1);
    digitalWrite(csPin, HIGH);
    delayMicroseconds(100);

    digitalWrite(csPin, LOW);
    delayMicroseconds(1);
    for (int i=0; i<N+4; ++i) {
      theSPI.transfer(0x00);
      // if (i==3 && c!= 0)
      // if (i >= 4) {
      //   buf[i-4] = c;
      // }
    }
    delayMicroseconds(1);
    digitalWrite(csPin, HIGH);
  }

  /**
   * @brief Retrieves angles and angular rates
   * @details This function blocks for a few hundred microseconds.
   * 
   * @param yaw in radians
   * @param pitch in radians
   * @param roll in radians
   * @param yawd in rad/s
   * @param pitchd in rad/s
   * @param rolld in rad/s
   */
  void get(float& yaw, float& pitch, float& roll, float& yawd, float& pitchd, float& rolld) {
    float dat[3];
    readReg(8, 12, (uint8_t *)dat);
    yaw = radians(dat[0]);
    pitch = radians(dat[1]);
    roll = radians(dat[2]);
    delayMicroseconds(30);
    readReg(19, 12, (uint8_t *)dat);
    yawd = dat[2];
    pitchd = dat[1];
    rolld = dat[0];
  }
};


#endif