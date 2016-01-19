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
#ifndef AS5047_h
#define AS5047_h

#include <SPI.h>

class AS5047 {
public:
  AS5047(SPI_TypeDef *SPIx, uint8_t cs) : SPIx(SPIx), cs(cs) {}

  uint16_t init() {
    pinMode(cs, OUTPUT);
    digitalWrite(cs, HIGH);
    // encoder setup
    uint16_t res = this->cmd(0x3fff, true);
    res = this->cmd(0x3fff, true);
    // return the absolute angle
    return res>>2;
  }

  uint16_t cmd(uint16_t dat, bool toRead) {
    if (toRead)
      bitSet(dat, 14);
    else
      bitClear(dat, 14);
      
    dat = addPARC(dat);
    
    uint16_t res;
    
    digitalWrite(cs, LOW);
    delayMicroseconds(1);
    SPI_I2S_SendData16(SPIx, dat);
    while (SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_RXNE) == RESET);
    res = SPI_I2S_ReceiveData16(SPIx);
    delayMicroseconds(1);
    digitalWrite(cs, HIGH);
    
    // remove parity from res by & with 0b11..11 (14 1's) = 0x3fff
    res = res & 0x3FFF;
    
    return res;
  }

protected:
  SPI_TypeDef *SPIx;
  const uint8_t cs;

  uint16_t addPARC(uint16_t dat) {
    unsigned char par = 0; 
    uint16_t dat1 = dat;
    while(dat1 != 0) { 
      par = par ^ 1; 
      dat1 &= (dat1-1); // the loop will execute once for each bit of ino set
    }
    if (par == 1)
      bitSet(dat, 15);
    else
      bitClear(dat, 15);
    return dat;
  }
};

#endif
