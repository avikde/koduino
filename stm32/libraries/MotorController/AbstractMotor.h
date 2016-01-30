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
#ifndef AbstractMotor_h
#define AbstractMotor_h

#include "Motor.h"

/** @addtogroup MotorController
 *  @{
 */

/**
 * @brief Class for coordinating N motors in parallel
 * 
 * @tparam N number of motors to be coordinated
 */
template<int N> 
class AbstractMotor {
public:
  // Constructor?
  // AbstractMotor()

  /**
   * @brief Transformation for forces at the toe to joint torques
   * @details To be overridden: coordinate transforms
   * 
   * @param in Toe forces
   * @param out Joint torques
   */
  virtual void physicalToAbstract(const float in[N], float out[N]) = 0;
  /**
   * @brief Forward kinematics
   * @param in Joint angles
   * @param out Toe position
   */
  virtual void abstractToPhysical(const float in[N], float out[N]) = 0;

  /**
   * @brief Enables all the motors in this "leg"
   * @param flag true enables, false disables
   */
  void enable(bool flag) {
    for (int i=0; i<N; ++i)
      motors[i]->enable(flag);
  }

  /**
   * @brief Returns the ith toe position coordinate
   * @details Unlike Motor::getPosition(), the AbstractMotor::getPosition() doesn't actually read sensors, but returns the value stored from the last time Motor::update() was called.
   * @param i ith entry in the forward kinematics as specified in abstractToPhysical()
   * @return Position in physical units
   */
  float getPosition(int i) { return pos[i]; }

  /**
   * @brief Returns the ith toe coordinate velocity
   * @param i ith entry in the forward kinematics as specified in abstractToPhysical()
   * @return Velocity in physical units
   */
  float getVelocity(int i) { return pd[i].vel; }

  /**
   * @brief Get the openloop value for the ith coordinate; useful when using setPosition(int, float)
   * @sa Motor::getOpenLoop()
   */
  float getOpenLoop(int i) {
    return this->val[i];
  }
  // Set motor move commands (but they are actually sent by motorUpdate())

  /**
   * @brief Set open loop command to coordinate i
   * @details Like Motor::setOpenLoop(), the actual communication happens in AbstractMotor::update()
   * @param val Between -1 and 1
   */
  void setOpenLoop(int i, float val) {
    this->mode[i] = OPEN_LOOP_MODE;
    this->val[i] = val;
  }

  /**
   * @brief Set PD gains for coordinate i
   * @param Kp P gain
   * @param Kd D gain (optional argument)
   */
  void setGain(int i, float Kp, float Kd) { pd[i].setGain(Kp, Kd); }
  void setGain(int i, float Kp) { setGain(i, Kp, 0); }

  /**
   * @brief Set position setpoint for coordinate i, and change to position control mode
   * @param setpoint Setpoint in units of coordinate i (PD control in end-effector space)
   */
  void setPosition(int i, float setpoint) {
    this->mode[i] = POSITION_MODE;
    this->setpoint[i] = setpoint;
  }

  /**
   * @brief Get position of 0th coordinate 
   * @sa getPosition(int)
   */
  virtual float getPosition() { return getPosition(0); }
  /**
   * @brief Get velocity of 0th coordinate
   * @sa getVelocity(int)
   */
  virtual float getVelocity() { return getVelocity(0); }
  /**
   * @brief Get open loop value of 0th coordinate 
   * @sa getOpenLoop(int)
   */
  virtual float getOpenLoop() { return getOpenLoop(0); }
  /**
   * @brief Set open loop value for 0th coordinate 
   * @sa setOpenLoop(int, float)
   */
  virtual void setOpenLoop(float val) { setOpenLoop(0, val); }
  /**
   * @brief Set position for 0th coordinate 
   * @sa setPosition(int, float)
   */
  virtual void setPosition(float pos) { setPosition(0, pos); }

  /**
   * @brief This should be called at a more or less fixed rate (once per iteration)
   * @details Don't call Motor::update() on the constituent motors
   */
  void update() {
    float posCtrlVal[N], physicalVal[N];

    for (int i=0; i<N; ++i)
      physicalPos[i] = motors[i]->getPosition();

    // call derived class function, and store positions in "pos" for getPosition()
    physicalToAbstract(physicalPos, pos);

    for (int i=0; i<N; ++i) {
      // Velocity calculation should happen independent of mode
      if (bAngle[i])
        posCtrlVal[i] = pd[i].update(fmodf_mpi_pi(pos[i] - setpoint[i]));
      else
        posCtrlVal[i] = pd[i].update(pos[i] - setpoint[i]);

      // In position mode, update the motor command
      if (mode[i] == POSITION_MODE)
        val[i] = posCtrlVal[i];
      // In open-loop mode, val has been set by user
    }

    // invert the coordinate change
    abstractToPhysical(val, physicalVal);

    for (int i=0; i<N; ++i) {
      // Send command, but don't modify "val" (set by user)
      correctedPhysicalVal[i] = motors[i]->mapVal(physicalVal[i]);
      // send the command
      motors[i]->sendOpenLoop(correctedPhysicalVal[i]);
    }
  }

  // Public access to motors
  Motor *motors[N];
  float val[N], correctedPhysicalVal[N], setpoint[N];
  bool bAngle[N];
protected:
  MotorControlMode mode[N];
  // Command
  float physicalPos[N], pos[N];
  // PD controller
  PD pd[N];
  // Barrier
  // Barrier barrier;
};


/** @} */ // end of addtogroup

#endif