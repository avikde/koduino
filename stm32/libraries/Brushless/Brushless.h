#ifndef Brushless_h
#define Brushless_h

#include <Arduino.h>
#include <Encoder.h>

enum CommutationType {
  BLOCK, SINUSOIDAL, TRAPEZOIDAL
};

enum ControlMode {
  VOLTAGE_CONTROL, POSITION_CONTROL, CURRENT_CONTROL, DEBUGGING
};

// TO use this class:
// 1. define a setOutputs function globally
void setOutputs(bool e1, float p1, bool e2, float p2, bool e3, float p3);
// 2. ??

class Brushless {
public:
  Encoder encoder;
  volatile float amplitude = 0;
  volatile float pos_act_01 = 0;
  volatile int commutationTime = 0;
  volatile float elang;
  volatile uint16_t currents[3];
  volatile int16_t current; // scalar combining phase currents
  volatile float posRad;

  // Has to be defined for each driver / feedback type combo
  // virtual void enable(bool flag) = 0;
  // Should use waveform from base class
  void setMotorPhases(float electricalAngle, float amplitude, CommutationType waveform);

  void setMotorProps(int polePairs, float leadFactor);

  void init(uint32_t absPos);
  void commutate();
  int16_t readCurrent();

  void calibrate(float sweepAmplitude);
  void update(float pwmInput);
  void setDebuggingAmplitude(float a) {debuggingAmplitude = a;}
  void setMode(ControlMode theMode) {mode = theMode;}
  void setWaveform(CommutationType wave) {waveform = wave;}

  float getPosition() { return posRad; }
  float getVelocity() { return motorVel; }

  void openLoopTest(uint32_t pause);

  // Modified from Piccoli
  void calcSpaceVector(float electricalAngle, float amplitude, uint8_t *enables, float *pwms, CommutationType commutation);

  // Helper
  float posToRadians(uint32_t pos);
  // This should be in EEPROM
  // TODO: write a initialization part that uses Serial to ask for this and any other motor-specific parameters?
  int pos_zer=0;
protected:
  DLPF velF;
  int POLE_PAIRS;
  float leadFactor = 0;
  float posctrl0 = 0, motorVel = 0, posDes = 0;
  float velInt = 0;
  bool motorEnableFlag = false;
  int countsPerElecRev = 0; // calculated
  volatile uint8_t section = 0;
  volatile int pos_act;
  float debuggingAmplitude;
  ControlMode mode;
  CommutationType waveform = SINUSOIDAL;
  uint16_t encoderCPR = 4096;
};


#endif
