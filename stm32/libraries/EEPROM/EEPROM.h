#ifndef EEPROM_h
#define EEPROM_h

#include <stdint.h>

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
