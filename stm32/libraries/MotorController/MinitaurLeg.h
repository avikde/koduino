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
#ifndef MinitaurLeg_h
#define MinitaurLeg_h

#include "AbstractMotor.h"


enum AbstractCoord {
  EXTENSION = 0, ANGLE = 1
};

// i=0->radial, i=1->tangential

/**
 * @brief Class for coordinating two parallel coaxial motors in the Minitaur config
 * @details Link lengths can be set, and kinematics are implemented. The user can choose to use meters for the extension, or a nonlinear mapping from 0 (folded up) to pi (all the way extended)
 */
class MinitaurLeg : public AbstractMotor<2> {
public:
  // If true, use the exact leg kinematics, and the extension coordinate is in length units. If false, use the mean angle as a proxy for extension (between 0 and PI)
  static bool useLengths;
  // upper and lower link lengths
  static float l1, l2;
  //constructor
  MinitaurLeg(Motor *M0, Motor *M1);

  // Forward kinematics
  void physicalToAbstract(const float joints[2], float toe[2]);
  // Map forces to joint torques
  void abstractToPhysical(const float toeForce[2], float jointTorque[2]);

  // Return forward speed
  float getSpeed(float bodyPitch);

protected:
  // this does the full FK irrespective of if useLengths is false
  float FKext(float meanAng);
};

#endif
