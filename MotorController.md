@addtogroup MotorController

### Usage

See example below.

Basic control of a single motor:
1. Call initialization function, like BlCon34::init() or DxlMotor::init()
2. Call Motor::enable()
3. Call Motor::setOpenLoop() with an argument between -1 and 1 to spin freely
4. Call Motor::setGain() and Motor::setPosition() to command a position
5. Call Motor::getPosition() or Motor::getVelocity() to get back data
6. Motor::update() must be called frequently, preferably through a timer interrupt (attachTimerInterrupt()) for BlCon34, or just as often as possible for DxlMotor

### Setting motor zero (independent of physical setup)

1. To calibrate a motor's zero position, rotate the output shaft of the motor (or gearbox) at the desired zero location.
2. Take the output of Motor::getRawPosition() (a number that will be between 0 and 2 PI), and use that as the zero argument for the Motor::init() (or the equivalent calls in specific derived classes, like BlCon34::init() or DxlMotor::init()). The direction of the motor and the gear box do not have any effect on the zeroing routine.
3. For startup, the motor's output shaft must be within (-PI, PI)/gearRatio of the calibrated zero position. This means that in direct drive, the motor can be at an arbitrary position, but when using a gearbox the startup angle is decreased since there are no absolute enocders.
4. If the motor driver setup takes some time (i.e. Motor::getRawPosition() returns garbage during init), then make sure to call Motor::resetOffset() after there is non-garbage data. This should not be an issue in PWM controlled motor controllers.
5. For any gear box with a ratio that is an integer, the motor/gear box assembly can be allowed to recirculate and still work correctly as long as the startup positioning is taken into account. However, for gear boxes with non-integer ratios (which seems to be most gear boxes), the motor/gear box assembly should NOT be allowed to recirculate ever because one full rotation of the output shaft results in a different Motor::getRawPosition() value.

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
