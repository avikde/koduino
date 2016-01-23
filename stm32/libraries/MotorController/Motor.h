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

/**
 * @brief Base single-motor class
 * @details Implements enable/disable, setting open-loop commands, PD position control, gear ratio. This class is an abstract base class, and derived classes need to implement the low-level communication with the motor driver.
 */
class Motor {
public:
  // Has to be defined for each motor driver / feedback type combo
  /**
   * @brief Enable the motor
   * @details Must be implemented in derived classes
   * 
   * @param flag true to enable, false to disable
   */
  virtual void enable(bool flag) = 0;
  /**
   * @brief Raw position read from the motor driver
   * @details This is before the direction/zero are applied. The user should call getPosition()
   * @return Raw position in [0, 2pi]
   */
  virtual float getRawPosition() = 0;
  /**
   * @brief Communicate a raw PWM to the motor controller
   * @details Must be implemented in derived classes. The user should call setOpenLoop()
   * 
   * @param val Raw PWM in [-1, 1]
   */
  virtual void sendOpenLoop(float val) = 0;

  // Initialization
  /**
   * @brief Initialize motor properties
   * 
   * @param zero The zero angle (in terms of getRawPosition())
   * @param direction One of `+1` and `-1` (reverses in which direction positions increase)
   * @param gearRatio Default is 1.0 (direct drive)
   */
  void init(float zero, int8_t direction, float gearRatio);
  void setBarrier(float ll, float ul);

  /**
   * @brief Get position after direction and zero are taken into account
   * @return Position in radians between -pi and pi
   */
  float getPosition();
  /**
   * @brief Get motor velocity after direction and zero are taken into account
   * @return Velocity in rad/s
   */
  float getVelocity();
  /**
   * @brief Get the PWM duty cycle commanded to the motor
   * @details Useful to record input when getPosition() is used
   * @return Commanded PWM in [-1, 1]
   */
  float getOpenLoop();
  /**
   * @brief Set an open loop PWM command for the motor
   * @details These commands are only sent by update()
   * 
   * @param val Commanded PWM in [-1, 1]
   */
  void setOpenLoop(float val);
  /**
   * @brief Set gain for position control
   * @details This must be called when setPosition() is used
   * 
   * @param Kp P-gain (in units of PWM / rad)
   * @param Kd D-gain (in units of PWM / (rad/s))
   */
  void setGain(float Kp, float Kd);
  /**
   * @brief Set P-gain for position control (D-gain is set to 0)
   * @details This must be called when setPosition() is used
   * 
   * @param Kp P-gain (in units of PWM / rad)
   */
  void setGain(float Kp) { setGain(Kp, 0); }
  /**
   * @brief Set a desired position for the motor in radians
   * @details setGain() needs to be called as well
   * 
   * @param setpoint Desired position in radians (relative to direction and zero set by Motor::init())
   */
  void setPosition(float setpoint);

  // Map output command based on direction, driverDirection
  float mapVal(float val);

  /**
   * @brief Function that communicates with the motor controller (including getting position, commanding PWM)
   * @details This should be called at a more or less fixed rate (once per iteration). Change Motor::updateRate to the expected frequency that update() will be called.
   * 
   * @return Returns the corrected raw PWM sent to the motor controller
   */
  float update();

  /**
   * @brief Run this after you are sure there is non-garbage rawPosition data
   */
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

/**
 * @brief Derived class for communicating with PWM-controlled boards
 * @details Uses PWM signals to send open loop commands, and receive position.
 */
class BlCon34 : public Motor {
public:
  static bool useEXTI;

  // Constructor (sets defaults)
  BlCon34() {}

  /**
   * @brief Initialize a motor with gear ratio 1
   * 
   * @param outPin_ PWM out pin
   * @param inPin_ PWM in pin
   * @param zero See Motor::init()
   * @param dir See Motor::init()
   * @param gearRatio See Motor::init()
   */
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

/** @} */ // end of addtogroup

#endif
