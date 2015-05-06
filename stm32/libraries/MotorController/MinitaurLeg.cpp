
#include "MinitaurLeg.h"

// defaults are for minitaur, can be overridden for centaur etc.
float MinitaurLeg::l1 = 10;
float MinitaurLeg::l2 = 20;

MinitaurLeg::MinitaurLeg(Motor *M0, Motor *M1) {
  motors[0] = M0;
  motors[1] = M1;

  // Set difference coordinate to be position controlled to 0
  setGain(1, 0.2);
  setPosition(1, 0);
}

// Helper function
void meanDiffAngles(const float motPos[2], float *meanAng, float *diffAng) {
  // a version of circleMeanDiff
  float r = fmodf_mpi_pi(motPos[0] - motPos[1]);
  *diffAng = 0.5 * r;
  // meanAng should be within PI
  *meanAng = fmodf(motPos[1] + (*diffAng), PI);
  if (*meanAng < 0.0)
    *meanAng += PI;
}

void MinitaurLeg::physicalToAbstract(const float joints[2], float toe[2]) {
  float meanAng = 0;
  meanDiffAngles(joints, &meanAng, &toe[ANGLE]);
  // convert mean angle to r
  float l1proj = l1*arm_sin_f32(meanAng);
  toe[EXTENSION] = sqrtf(l2*l2 - l1proj*l1proj) - l1*arm_cos_f32(meanAng);
}

void MinitaurLeg::abstractToPhysical(const float f[2], float tau[2]) {
  // Use jacobian?
  // float meanAng = 0, diffAng = 0;
  // meanDiffAngles(physicalPos, &meanAng, &diffAng);

  // float fmean = (l1*arm_sin_f32(meanAng) - l1*l1*arm_cos_f32(meanAng)*arm_sin_f32(meanAng)/sqrtf(l2*l2 - l1*l1*arm_sin_f32(meanAng)*arm_sin_f32(meanAng))) * f[EXTENSION];

  // Invert the mean/diff coordinate change
  tau[0] = f[EXTENSION] + f[ANGLE];
  tau[1] = f[EXTENSION] - f[ANGLE];
}

