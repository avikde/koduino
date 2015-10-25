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
#ifndef SagittalPair_h
#define SagittalPair_h

#include "AbstractMotor.h"

/** @addtogroup MotorController
 *  @{
 */

// i=0->mean, i=1->diff coordinate
class SagittalPair : public AbstractMotor<2> {
public:
  //constructor
  SagittalPair(Motor *M0, Motor *M1) {
    motors[0] = M0;
    motors[1] = M1;

    // Set difference coordinate to be position controlled to 0
    setGain(1, 1);
    setPosition(1, 0);
  }

  // This should be overriding a base class function
  void physicalToAbstract(const float in[2], float out[2]) {
    circleMeanDiff(in[0], in[1], &out[0], &out[1]);
  }

  void abstractToPhysical(const float in[2], float out[2]) {
    // Invert the mean/diff coordinate change
    out[0] = in[0] + in[1];
    out[1] = in[0] - in[1];
  }

  // New commands for setting what the difference should do
  void setDiffGain(float Kp, float Kd) { setGain(1, Kp, Kd); }
  void setDiffGain(float Kp) { setDiffGain(Kp, 0); }
};

/** @} */ // end of addtogroup

#endif
