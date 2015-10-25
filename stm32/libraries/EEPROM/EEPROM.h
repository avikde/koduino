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
#ifndef EEPROM_h
#define EEPROM_h

#include <stdint.h>

/**
 * @brief EEPROM library (call with global object `EEPROM`)
 * @details 
 */
class EEPROMClass
{
public:
  EEPROMClass(void) : inited(false) {}
  // uint16_t init(void);
  uint16_t read (uint16_t address);
  void write(uint16_t address, uint16_t data);
  // Not implemented but would be useful--maple library has them
  uint16_t count(uint16_t *);
  uint16_t maxcount(void);
protected:
  void initCheck();
  bool inited;
};

extern EEPROMClass EEPROM;

#endif
