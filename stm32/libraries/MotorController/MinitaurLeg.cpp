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
#include "MinitaurLeg.h"

// defaults are for minitaur, can be overridden for centaur etc.
float MinitaurLeg::l1 = 0.1;
float MinitaurLeg::l2 = 0.2;
bool MinitaurLeg::useLengths = false;

MinitaurLeg::MinitaurLeg(Motor *M0, Motor *M1) {
  motors[0] = M0;
  motors[1] = M1;

  bAngle[EXTENSION] = false;
  bAngle[ANGLE] = true;

  // Set difference coordinate to be position controlled to 0
  setGain(1, 0.2);
  setPosition(1, 0);
}

// Helper function
// returns meanAng in [0,PI], diffAng in [-PI/2, PI/2]
void meanDiffAngles(const float motPos[2], float *meanAng, float *diffAng) {
  // a version of circleMeanDiff
  // float r = fmodf_mpi_pi(motPos[0] - motPos[1]);
  float r = motPos[0] - motPos[1];
  *diffAng = 0.5 * r;
  *meanAng = fmodf(motPos[1] + (*diffAng), PI);
  if (*meanAng < 0.0) {
    *meanAng += PI;
    *diffAng = fmodf_mpi_pi(*diffAng + PI);
  }
}

float MinitaurLeg::FKext(float meanAng) {
  // convert mean angle to r
  float l1proj = l1*arm_sin_f32(meanAng);
  return sqrtf(l2*l2 - l1proj*l1proj) - l1*arm_cos_f32(meanAng);
}

void MinitaurLeg::physicalToAbstract(const float joints[2], float toe[2]) {
  float meanAng = 0;
  meanDiffAngles(joints, &meanAng, &toe[ANGLE]);
  toe[EXTENSION] = (useLengths) ? FKext(meanAng) : meanAng;
}

void MinitaurLeg::abstractToPhysical(const float toeForce[2], float jointTorque[2]) {
  float fmean = 0;
  if (useLengths) {
    // Use jacobian?
    float meanAng = 0, diffAng = 0;
    meanDiffAngles(physicalPos, &meanAng, &diffAng);

    float l1proj = l1*arm_sin_f32(meanAng);
    fmean = (l1*arm_sin_f32(meanAng) - l1*l1*arm_cos_f32(meanAng)*arm_sin_f32(meanAng)/sqrtf(l2*l2 - l1proj*l1proj)) * toeForce[EXTENSION];
  } else {
    fmean = toeForce[EXTENSION];
  }
  // Invert the mean/diff coordinate change
  jointTorque[0] = fmean + toeForce[ANGLE];
  jointTorque[1] = fmean - toeForce[ANGLE];
}

float MinitaurLeg::getSpeed(float bodyPitch) {
  float theta = getPosition(ANGLE) + bodyPitch, thetadot = getVelocity(ANGLE);
  float r, rdot;
  if (useLengths) {
    r = getPosition(EXTENSION);
    rdot = getVelocity(EXTENSION);
  } else {
    float meanAng = getPosition(EXTENSION), meanAngDot = getVelocity(EXTENSION);
    // Copying FKext here to reuse some terms
    float l1proj = l1*arm_sin_f32(meanAng);
    float term1 = sqrtf(l2*l2 - l1proj*l1proj), term2 = l1*arm_cos_f32(meanAng);
    r = term1 - term2;
    // jacobian
    float DFKext = l1proj * (1 - term2 / term1);
    rdot = DFKext * meanAngDot;
  }

  return rdot * arm_sin_f32(theta) + r * arm_cos_f32(theta) * thetadot;
}

void MinitaurLeg::getToeForce(float& ur, float& uth) {
  float u0 = motors[0]->getTorque();
  float u1 = motors[1]->getTorque();
  
  float meanAng = 0, diffAng = 0;
  meanDiffAngles(physicalPos, &meanAng, &diffAng);
  float l1proj = l1*arm_sin_f32(meanAng);
  float dummy = -2/(-2*l1proj + l1*l1*arm_sin_f32(physicalPos[0] + physicalPos[1])/sqrtf(l2*l2 - l1proj*l1proj));
  ur = dummy * (u0+u1);
  uth = u0-u1;
}
