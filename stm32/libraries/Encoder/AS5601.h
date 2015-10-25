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
#ifndef AS5601_h
#define AS5601_h

class AS5601 {
public:
  static const uint8_t ADDRESS = 0x36;
  static const uint8_t STATUS = 0x0b;
  static const uint8_t ABN = 0x09;
  static const uint8_t ANGLEH = 0x0e;
  static const uint8_t ANGLEL = 0x0f;
  
  AS5601() {}
  void init() {
    Wire.setSpeed(WIRE_1M);
    Wire.begin();
  }
  
  void write(uint8_t reg, uint8_t val) {
    Wire.beginTransmission(ADDRESS);
    Wire.write(reg);
    Wire.write(val);
    Wire.endTransmission();
  }
  
  uint8_t read(uint8_t reg) {
    Wire.beginTransmission(ADDRESS);
    Wire.write(reg);
    Wire.endTransmission(false);
    Wire.requestFrom(ADDRESS, 1);
    return Wire.read();
  }
  
  uint16_t getAngle() {
    uint16_t angl = read(AS5601::ANGLEL);
    uint16_t angh = read(AS5601::ANGLEH);
    // convert from 12-bit to 11-bit
    return ((angh<<8)+angl)>>1;
  }
};

#endif
