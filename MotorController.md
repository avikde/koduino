@addtogroup MotorController

### Usage

1. TODO. mostly, see example

### Setting motor zero for various directions and/or gear ratios

1. TODO Garrett

### Example: Motor test

~~~{.cpp}

#include <Motor.h>

// All connectors on Mainboard v1.1
const uint8_t pwm1 = PA0;
const uint8_t pulse1 = PA6;
const uint8_t pwm2 = PA11;
const uint8_t pulse2 = PB5;
const uint8_t pwm3 = PA2;
const uint8_t pulse3 = PB6;
const uint8_t pwm4 = PA3;
const uint8_t pulse4 = PB7;
const uint8_t pwm5 = PB0;
const uint8_t pulse5 = PB1;
const uint8_t pwm6 = PB8;
const uint8_t pulse6 = PB9;
const uint8_t pwm7 = PA14;
const uint8_t pulse7 = PA15;
const uint8_t pwm8 = PA1;
const uint8_t pulse8 = PA5;

const int CONTROL_RATE = 1000;
BlCon34 M1;

void controlLoop() {
  M1.update();
}

void setup() {
  Serial1.begin(115200);
  pinMode(led, OUTPUT);

  Motor::updateRate = CONTROL_RATE;
  Motor::velSmooth = 0.55;

  // Arguments are pwmPin, pulsePin, zero(rad), direction(+/- 1)
  M1.init(pwm1, pulse1, 0, 1);

  // Also can specify a gearRatio (e.g. 23)
  // M1.init(pwm1, pulse1, 0, 1, 23);

  attachTimerInterrupt(0, controlLoop, CONTROL_RATE);
}

void loop() {
  // set P-gain of 0.5/rad
  M1.setGain(0.5);
  // can also set D-gain like the following:
  // M1.setGain(0.5, 0.01);

  // Must call this to enable motor controller
  M1.enable(true);

  // go to 0 radians
  M1.setPosition(0);
  delay(1000);

  // go to 1 radians
  M1.setPosition(1);
  delay(1000);

  // spin freely in the + direction
  M1.setOpenLoop(0.5);
  delay(1000);

  // spin freely in the - direction
  M1.setOpenLoop(-0.5);
  delay(1000);
}

~~~
