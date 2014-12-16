@addtogroup EXTI

### Usage

1. Call attachInterrupt()

### Example: Mass transport sensor

~~~{.cpp}

#include <Arduino.h>

// Change to whatever pin an LED is connected to
const int led = PC13;
// Declare variables that will be modified by interrupt handlers `volatile`
volatile int count = 0;

void countUp() {
  count++;
}

void setup() {
  Serial1.begin(115200);
  attachInterrupt(PA0, countUp, CHANGE);
}

void loop() {
  Serial1 << "Counted " << count << " particles.\n";
  delay(100);
}

~~~
