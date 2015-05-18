#ifndef MinitaurLeg_h
#define MinitaurLeg_h

#include "AbstractMotor.h"


enum AbstractCoord {
  EXTENSION = 0, ANGLE = 1
};

// i=0->radial, i=1->tangential
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
