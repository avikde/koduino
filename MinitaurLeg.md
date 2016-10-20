@class MinitaurLeg MinitaurLeg.h

This class is derived from AbstractMotor, in particular it coordinates two motors together in a parallel five-bar configuration. 

### Kinematics

The kinematics of the leg can be found in http://kodlab.seas.upenn.edu/Gavin/IROS15. In particular, Fig. 1 (borrowed from the paper and included below) will be helpful to follow this help page.

![Minitaur leg kinematics definitions](../MinitaurLeg.PNG "Minitaur leg kinematics definitions")

The end-effector coordinates (first argument to MinitaurLeg::setOpenLoop(), MinitaurLeg::setGain(), and MinitaurLeg::setPosition()) are `EXTENSION` and `ANGLE`. Assuming the inidividual motor zeros are set when the upper links are pointing vertically up, and the motor directions are set so the positions increase as the motors sweep down (this is the default of Minitaur),
* The `ANGLE` coordinate always refers to _α - π_ in radians (where the front of the robot is to the right in the figure above). I.e., when the leg is vertically pointing down, `ANGLE` is 0, if it sweeps back, `ANGLE` is positive, etc.
* When MinitaurLeg::useLengths is false (this is default), the `EXTENSION` coordinate either refers to _β_ in radians from 0 (fully retracted) to _π_ (fully extended)
* When MinitaurLeg::useLengths is true, the `EXTENSION` coordinate refers to _r_ in meters.


### Usage

To use a MinitaurLeg in your code, follow the usage steps in the AbstractMotor help page. Additionally,
1. Set MinitaurLeg::useLengths to true if you would like to use the _r_ coordinate instead of _β_ for the leg extension
2. Set the upper link length MinitaurLeg::l1 (default is 0.1 m) and the lower link length MinitaurLeg::l2 (default is 0.2 m)
3. You can call MinitaurLeg::getSpeed() to get a body speed estimate if you supply the body pitch (leaning forward is assumed to be positive pitch).
4. To get proprioceptive torque estimates, you can still use Motor::getTorque() as long as Motor::setTorqueEstParams() has been called.

### Example

~~~{.cpp}

#include <Motor.h>
#include <MinitaurLeg.h>

// Uncomment the lines corresponding to your board
// MBLC 0.5
const int NMOT = 10;
const uint8_t outPin[] = {PE9, PE11, PE13, PE14, PA0, PD4, PD7, PD6, PB4, PB5};
const uint8_t inPin[] = {PD12, PD13, PD14, PD15, PC6, PC7, PC8, PC9, PE2, PE3};
// // Mainboard v1.1
// const int NMOT = 8;
// const uint8_t outPin[] = {PA0, PA11, PA2, PA3, PB0, PB8, PA14, PA1};
// const uint8_t inPin[] = {PA6, PB5, PB6, PB7, PB1, PB9, PA15, PA5};
// // Mainboard v2
// const int NMOT = 8;
// const uint8_t outPin[] = {PA3, PA2, PA0, PA1, PB0, PB1, PA6, PB5};
// const uint8_t inPin[] = {PB8, PB9, PB3, PA8, PA11, PA15, PB14, PB15};


const int CONTROL_RATE = 1000;
BlCon34 M[NMOT];
MinitaurLeg leg0(&M[1], &M[0]);

void controlLoop() {
  leg0.update();
}

void setup() {
  Serial1.begin(115200);

  Motor::updateRate = CONTROL_RATE;
  Motor::velSmooth = 0.9;
  // Uncomment this for MBLC
  // BlCon34::useEXTI = true;

  // Arguments are pwmPin, pulsePin, zero(rad), direction(+/- 1)
  for (int i=0; i<NMOT; ++i)
    M[i].init(outPin[i], inPin[i], 0, 1);

  attachTimerInterrupt(0, controlLoop, CONTROL_RATE);
}

void loop() {
  leg0.enable(true);
  // These commands are similar to Motor commands, except with an added first argument
  leg0.setGain(ANGLE, 0.3);
  leg0.setPosition(ANGLE, 0);

  leg0.setOpenLoop(EXTENSION, 0.1);
}

~~~
