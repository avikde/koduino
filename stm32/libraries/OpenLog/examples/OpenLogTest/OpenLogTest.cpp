
#include <Arduino.h>
#include <OpenLog.h>

const PinName led = PC13;

// Define logging
struct LogRow {
  uint32_t t;
  float num;
} __attribute__ ((packed));
LogRow logRow;
OpenLog openLog(PB2, Serial2, 115200);

void setup() {
  pinMode(led, OUTPUT);
  openLog.init("t,msg", "If", sizeof(logRow));
  openLog.enable(true);
}

void loop() {
  digitalWrite(led, TOGGLE);

  logRow.t = millis();
  logRow.num = 1.234;
  openLog.write((const uint8_t *)&logRow);
  delay(10);
}
