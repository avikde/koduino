@addtogroup Time

### Usage

1. Call millis() or micros() to know the current time
2. Call delay(), delayNB() or delayMicroseconds() to block execution for a duration

### Example: Blink

~~~{.cpp}

#include <Arduino.h>

// Change to whatever pin an LED is connected to
const int led = PC13;

void setup() {
  pinMode(led, OUTPUT);
}

void loop() {
  digitalWrite(led, TOGGLE);
  delay(1000);
}

~~~
