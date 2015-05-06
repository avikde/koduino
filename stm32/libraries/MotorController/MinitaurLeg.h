#ifndef MinitaurLeg_h
#define MinitaurLeg_h

#include "AbstractMotor.h"


enum AbstractCoord {
  EXTENSION = 0, ANGLE = 1
};

// i=0->radial, i=1->tangential
class MinitaurLeg : public AbstractMotor<2> {
public:
  // upper and lower link lengths
  static float l1, l2;
  //constructor
  MinitaurLeg(Motor *M0, Motor *M1);

  // Forward kinematics
  void physicalToAbstract(const float joints[2], float toe[2]);
  // Map forces to joint torques
  void abstractToPhysical(const float f[2], float tau[2]);
};

#endif
