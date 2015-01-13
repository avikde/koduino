@addtogroup PWM

### Usage

PWM output:

1. (Optionally) call pinRemap() to change the timer connected to a pin
2. (Optionally) call analogWriteFrequency() to change output PWM frequency
3. Call pinMode() with `PWM`
4. Call analogWrite()

PWM input:

1. (Optionally) call pinRemap() to change the timer connected to a pin
2. Call pinMode() with `PWM_IN`
3. Call pwmIn()

## Example: PWM output

~~~{.cpp}

#include <Arduino.h>

void setup() {
  // (Optional) non Arduino-like function to assign a different timer to PB6
  pinRemap(PB6, 11, TIMER19, 1);

  // Arduino-like configuration for PWM
  analogWriteFrequency(PB6, 25000);
  // Unlike in Arduino, this *MUST* be called before analogWrite()
  pinMode(PB6, PWM);
}

void loop() {
  // 75% duty cycle PWM at 25 KHz
  analogWrite(PB6, 0.75);
} 

~~~

## Example: PWM input (loopback) test

~~~{.cpp}

#include <Arduino.h>

// Connect PA0 and PA6 for this test

void setup() {
  Serial1.begin(115200);
  pinMode(PA0, PWM);
  pinMode(PA6, PWM_IN);
}

void loop() {
  analogWrite(PA0, 0.3);
  Serial1 << "Read " << pwmIn(PA6) << "\n";
  delay(10);
}

~~~
