#ifndef AbstractMotor_h
#define AbstractMotor_h

#include "Motor.h"

// cldoc:begin-category(MotorController)

// This should be templated
// Class for two motors in parallel: difference in position is position-controlled and the gain can be set. The mean can be open-loop or position-controlled.
template<int N> 
class AbstractMotor {
public:
  // Constructor?
  // AbstractMotor()

  // To be overridden: coordinate transforms
  // Take positions from physical motors to abstract motors
  virtual void physicalToAbstract(const float in[N], float out[N]) = 0;
  // Take command values from abstract motor to physical motor coords (inverse of above)
  virtual void abstractToPhysical(const float in[N], float out[N]) = 0;

  void enable(bool flag) {
    for (int i=0; i<N; ++i)
      motors[i]->enable(flag);
  }
  // Get*() functions
  // Unlike the "Motor" class, the getPosition doesn't actually read sensors, but returns the value stored from the last time update() was called
  float getPosition(int i) { return pos[i]; }
  float getVelocity(int i) { return pd[i].vel; }

  float getOpenLoop(int i) {
    return this->val[i];
  }
  // Set motor move commands (but they are actually sent by motorUpdate())
  void setOpenLoop(int i, float val) {
    this->mode[i] = OPEN_LOOP_MODE;
    this->val[i] = val;
  }

  void setGain(int i, float Kp, float Kd) { pd[i].setGain(Kp, Kd); }
  void setGain(int i, float Kp) { setGain(i, Kp, 0); }
  void setPosition(int i, float setpoint) {
    this->mode[i] = POSITION_MODE;
    this->setpoint[i] = setpoint;
  }

  // Shortcuts for when no i is specified: use 0
  virtual float getPosition() { return getPosition(0); }
  virtual float getVelocity() { return getVelocity(0); }
  virtual float getOpenLoop() { return getOpenLoop(0); }
  virtual void setOpenLoop(float val) { setOpenLoop(0, val); }
  virtual void setPosition(float pos) { setPosition(0, pos); }

  // This should be called at a more or less fixed rate (once per iteration)
  void update() {
    float physicalPos[N], posCtrlVal[N], physicalVal[N];

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
  float pos[N];
  // PD controller
  PD pd[N];
  // Barrier
  // Barrier barrier;
};

// cldoc:end-category()


#endif