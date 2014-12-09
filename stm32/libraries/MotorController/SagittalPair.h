#ifndef SagittalPair_h
#define SagittalPair_h

#include "AbstractMotor.h"

// cldoc:begin-category(MotorController)

// i=0->mean, i=1->diff coordinate
class SagittalPair : public AbstractMotor<2> {
public:
  //constructor
  SagittalPair(Motor *M0, Motor *M1) {
    motors[0] = M0;
    motors[1] = M1;

    // Set difference coordinate to be position controlled to 0
    setGain(1, 2);
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

// cldoc:end-category()

#endif
