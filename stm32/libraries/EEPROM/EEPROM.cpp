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
#include "EEPROM.h"
#include "eeprom_stm32.h"


EEPROMClass EEPROM;


/**
 * @brief Read a 16-bit integer from EEPROM
 * @details 
 * 
 * @param address Address between 0 and 31
 * @return 
 */
uint16_t EEPROMClass::read(uint16_t address) {
  initCheck();
  static uint16_t ret;
  EE_ReadVariable(address, &ret);
  return ret;
}

/**
 * @brief Store a 16-bit integer in EEPROM
 * @details
 * 
 * @param address Address between 0 and 31
 * @param value 
 */
void EEPROMClass::write(uint16_t address, uint16_t value) {
  initCheck();
  EE_WriteVariable(address, value);
}

void EEPROMClass::initCheck() {
  if (!inited) {
    EE_Init();
    inited = true;
  }
}

