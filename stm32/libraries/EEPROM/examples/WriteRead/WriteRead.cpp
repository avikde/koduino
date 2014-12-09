
#include <wirish.h>
#include <EEPROM.h>

void setup()
{
  Serial1.begin(115200);
  EEPROM.write(0, 123);
  EEPROM.write(1, 456);
}

void loop()
{
  Serial1.printf("Expect 123 456, read %d %d.\n", EEPROM.read(0), EEPROM.read(1));
  delay(1000);
}
