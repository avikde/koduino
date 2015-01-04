
#include "EEPROM.h"
#include "eeprom_stm32.h"

uint16_t EEPROMClass::read(uint16_t address) {
  initCheck();
  static uint16_t ret;
  EE_ReadVariable(address, &ret);
  return ret;
}

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


EEPROMClass EEPROM;
