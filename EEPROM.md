@class EEPROMClass EEPROM.h

This class allows storing data / parameters in non-volatile memory that you may not want to hard-code in the source file, e.g. the zero-position for a motor. *Note:* unlike Arduino, in which each address corresponds to a byte, here each address from 0-31 corresponds to a 32-bit value (convenient for ints and floats).

### Usage

1. Call EEPROMClass::write() to store, and EEPROMClass::read() to retrieve values

### Example: Write/read

~~~{.cpp}

#include <EEPROM.h>

void setup() {
  Serial1.begin(115200);
  EEPROM.write(0, 123);
  EEPROM.write(1, 456);
}

void loop() {
  Serial1 << "Expect 123, 456; read " << EEPROM.read(0) << ", " << EEPROM.read(1) << "\n";
  delay(1000);
}
~~~
