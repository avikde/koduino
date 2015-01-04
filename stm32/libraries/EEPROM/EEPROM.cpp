
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

