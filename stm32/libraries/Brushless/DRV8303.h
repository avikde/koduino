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
#ifndef DRV8303_h
#define DRV8303_h

#include <SPI.h>

class DRV8303 {
public:
  DRV8303(SPI_TypeDef *SPIx, uint8_t cs) : SPIx(SPIx), cs(cs) {
    csConfigured = false;
  }

  void init() {
    if (!csConfigured) {
      pinMode(cs, OUTPUT);
      digitalWrite(cs, HIGH);
      csConfigured = true;
    }
    // disable OCP, set to dependent PWM, reset gate
    uint16_t cr1 = 0b11111111100;//0b111100;
    cmd(cr1, 0x02, false);
  }

  // assume this command will be called over and over
  bool checkConfig() {
    // if PVDD is not > 6V this will just return garbage, so try to read this register back to make sure initialization worked
    return (read(0x02) == 0b1011111111000);//0b1000000111000);
    // return (this->cmd(0, 0, true) == 0);
  }

  uint16_t cmd(uint16_t dat, uint8_t reg, bool toRead) {
    // mask so that dat only has the last 11 bits
    dat = dat & 0b11111111111;
    // address is bit 14:11
    dat |= ((uint16_t)(reg & 0b1111))<<11;
    // bit15 is R/W 
    if (toRead)
      bitSet(dat, 15);
    else
      bitClear(dat, 15);
    
    uint16_t res;
    
    digitalWrite(cs, LOW);
    delayMicroseconds(1);
    SPI_I2S_SendData16(SPIx, dat);
    while (SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_RXNE) == RESET);
    res = SPI_I2S_ReceiveData16(SPIx);
    delayMicroseconds(1);
    digitalWrite(cs, HIGH);
    
    return res;
  }

  uint16_t read(uint8_t reg) {
    this->cmd(0, reg, true);
    return this->cmd(0, 0, true);
  }

protected:
  SPI_TypeDef *SPIx;
  const uint8_t cs;
  bool csConfigured;
};

#endif
