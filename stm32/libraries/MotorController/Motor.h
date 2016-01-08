/**
 * @authors Avik De <avikde@gmail.com>

  This file is part of koduino <https://github.com/avikde/koduino>

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation, either
  version 3 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, see <http://www.gnu.org/licenses/>.
 */
#ifndef Motor_h
#define Motor_h

#include <Arduino.h>

/** @addtogroup MotorController Motor controller library
 *  @{
 */

// ===============================================================================
// New types
// ===============================================================================

enum MotorControlMode {
  OPEN_LOOP_MODE, POSITION_MODE
};

struct MotorConnector {
  uint8_t outPin, inPin;
  MotorConnector(uint8_t o, uint8_t i) : outPin(o), inPin(i) {}
} __attribute__ ((packed));

// cldoc:begin-category(MotorController)

// ===============================================================================
// Helper class to implement software barrier
// ===============================================================================

class Barrier {
public:
  void init() {
    enabled = false;
    ll = ul = 0;
  }

  // TODO: implement some hysteresis to prevent jittering
  float calculate(float pos) {
    if (!enabled)
      return 0;

    const static float maxVal = 1.0;

    // FIXME: differences on a circle?? can't just use >, <
    float barrier = - (1.0/(ul - pos) - 1.0/(pos - ll));
    if (pos > ul)
      barrier = -maxVal;
    if (pos < ll)
      barrier = maxVal;
    return barrier;
  }

  // members
  bool enabled;
  float ll, ul;
};

// ===============================================================================
// Base class
// ===============================================================================

class Motor {
public:
  // Has to be defined for each motor driver / feedback type combo
  virtual void enable(bool flag) = 0;
  virtual float getRawPosition() = 0;
  virtual void sendOpenLoop(float val) = 0;

  // Initialization
  void init(float zero, int8_t direction, float gearRatio);
  void setBarrier(float ll, float ul);
  // Get*() functions
  float getPosition();
  float getVelocity();
  float getOpenLoop();
  // Set motor move commands (but they are actually sent by motorUpdate())
  void setOpenLoop(float val);
  void setGain(float Kp, float Kd);
  void setGain(float Kp) { setGain(Kp, 0); }
  void setPosition(float setpoint);
  // Map output command based on direction, driverDirection
  float mapVal(float val);
  // This should be called at a more or less fixed rate (once per iteration)
  float update();
  // run this after you are sure there is non-garbage rawPosition data
  void resetOffset();

  // Static params - for every instance
  static int updateRate;
  static float velSmooth, rpsLimit;

  float gearRatio, prevPosition;
  int unwrapOffset;
  
  // used to ignore readings corresponding to physically impossible changes in motor position 
  float posLimit;

protected:
  MotorControlMode mode;
  // Properties
  float zero;
  // +/-1. relates what the user wants to be forward with the encoder
  int8_t direction;
  // +/-1. Relates encoder forward with LOW dirPin or +ve PWM drive signal
  int8_t driverDirection;
  // Reduction ratio (AFTER the encoder), i.e. > 1 if there is a reduction. The motor must be started near 0.
  // Command
  float val, correctedVal, setpoint;
  // PD controller
  PD pd;
  // Barrier
  Barrier barrier;
  //
  bool enableFlag;
  //
  float curPos;
};

// ===============================================================================
// Derived classes: driver / feedback device specific
// ===============================================================================

class BlCon34 : public Motor {
public:
  static bool useEXTI;

  // Constructor (sets defaults)
  BlCon34() {}

  // "Constructors"
  void init(uint8_t outPin_, uint8_t inPin_, float zero, int8_t dir, float gearRatio);
  void init(uint8_t outPin_, uint8_t inPin_, float zero, int8_t dir) {
    init(outPin_, inPin_, zero, dir, 1.0);
  }
  void init(const MotorConnector& J, float zero, int8_t dir, float gearRatio) {
    init(J.outPin, J.inPin, zero, dir, gearRatio);
  }
  void init(const MotorConnector& J, float zero, int8_t dir) {
    init(J.outPin, J.inPin, zero, dir, 1.0);
  }
  // From base class
  void enable(bool flag);
  float getRawPosition();
  void sendOpenLoop(float val);

protected:
  void initCommon(uint8_t outPin_, float zero, int8_t dir, float gearRatio);
  
  uint8_t outPin, inPin;
  // bool usePwmIn;
};

/** @} */ // end of addtogroup


// class BlCon3 : public Motor {
// public:
//   // "Constructors"
//   void init(PinName pwmPin, PinName dPin, PinName inPin) {
//     this->pwmPin = pwmPin;
//     this->dPin = dPin;
//     this->inPin = inPin;
//     analogWrite(outPin, 0.5);
//     driverDirection = 1;
//   }

//   // From base class
//   void enable(bool flag) {
//     digitalWrite(dPin, (flag) ? HIGH : LOW);
//   }
//   float getRawPosition() {
//     return (pwmIn(inPin) * TWO_PI);
//   }
//   void sendOpenLoop(float val) {
//     analogWrite(pwmPin, map(val, -1, 1, 0.1, 0.9));
//   }

// protected:
//   PinName pwmPin, dPin, inPin;
// };


// class PololuHP : public Motor {
// public:
//   // "Constructors"
//   void init(PinName pwmPin, PinName dirPin, PinName rstPin, PinName inPin) {
//     this->pwmPin = pwmPin;
//     this->dPin = dPin;
//     this->inPin = inPin;
//     analogWrite(outPin, 0.5);
//     driverDirection = 1;
//   }

//   // From base class
//   void enable(bool flag) {
//     digitalWrite(dPin, (flag) ? HIGH : LOW);
//   }
//   float getRawPosition() {
//     return (pwmIn(inPin) * TWO_PI);
//   }
//   void sendOpenLoop(float val) {
//     analogWrite(pwmPin, map(val, -1, 1, 0.1, 0.9));
//   }

// protected:
//   PinName pwmPin, dirPin, rstPin;
// };

#endif
