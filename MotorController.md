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

1. Rotate the output shaft of the motor (or gearbox) to the desired zero location.
2. Take the output of Motor::getRawPosition() (between 0 and 2 PI), and use that as the `zero` argument for the initialization function (BlCon34::init(), DxlMotor::init(), etc). The `direction` and `gearRatio` do not have any effect on the zeroing routine.
3. For robot startup, the motor's output shaft must be within (-PI, PI)/gearRatio of the calibrated zero position. This means that in direct drive, the motor can be at an arbitrary position, but when using a gearbox the startup angle is decreased since there are no absolute enocders.
4. If the motor driver setup takes some time (i.e. Motor::getRawPosition() returns garbage during init), then make sure to call Motor::resetOffset() after there is non-garbage data. This should not be an issue in PWM controlled motor controllers.
5. For any gearbox with an integer ratio, the leg can recirculate and still work correctly. However, for gearboxes with non-integer ratios, the motor/gearbox assembly should NOT be allowed to recirculate ever because one full rotation of the output shaft results in a different Motor::getRawPosition() value.

### Example: Motor test

~~~{.cpp}

#include <Motor.h>

// Uncomment the lines corresponding to your board
// Mainboard v1.1
const int NMOT = 8;
const uint8_t outPin[] = {PA0, PA11, PA2, PA3, PB0, PB8, PA14, PA1};
const uint8_t inPin[] = {PA6, PB5, PB6, PB7, PB1, PB9, PA15, PA5};
// // Mainboard v2
// const int NMOT = 8;
// const uint8_t outPin[] = {PA3, PA2, PA0, PA1, PB0, PB1, PA6, PB5};
// const uint8_t inPin[] = {PB8, PB9, PB3, PA8, PA11, PA15, PB14, PB15};
// // MBLC
// const int NMOT = 10;
// const uint8_t outPin[] = {PA5, PA1, PB10, PB11, PC0, PC1, PC2, PC3, PB4, PB5};
// const uint8_t inPin[] = {PF1, PB2, PB0, PC4, PC5, PC6, PC7, PC8, PA11, PA12};

const int CONTROL_RATE = 1000;
BlCon34 M[NMOT];

void controlLoop() {
  for (int i=0; i<NMOT; ++i)
    M[i].update();
}

void setup() {
  Serial1.begin(115200);

  Motor::updateRate = CONTROL_RATE;
  Motor::velSmooth = 0.9;
  // Uncomment this for MBLC
  // BlCon34::useEXTI = true;

  // Arguments are pwmPin, pulsePin, zero(rad), direction(+/- 1)
  for (int i=0; i<NMOT; ++i) {
    M[i].init(outPin[i], inPin[i], 0, 1);
    // Also can specify a gear ratio (e.g. 23)
    // M[i].init(outPin[i], inPin[i], 0, 1, 23);

    // set P-gain of 1.0/rad. This can be called as often as needed.
    M[i].setGain(1.0);
    // can also set D-gain like the following:
    // M[i].setGain(1.0, 0.01);
  }

  attachTimerInterrupt(0, controlLoop, CONTROL_RATE);
}

void loop() {
  // Must call this to enable motor controller
  M[0].enable(true);

  // go to 0 radians
  M[0].setPosition(0);
  delay(1000);

  // go to 1 radians
  M[0].setPosition(1);
  delay(1000);

  // spin freely in the + direction (gains are ignored in open loop mode)
  M[0].setOpenLoop(0.5);
  delay(1000);

  // spin freely in the - direction
  M[0].setOpenLoop(-0.5);
  delay(1000);
}

~~~
