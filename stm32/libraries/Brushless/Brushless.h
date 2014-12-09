#ifndef Brushless_h
#define Brushless_h

#include <stdint.h>
#include <Arduino.h>
#include <Encoder.h>

#ifdef __cplusplus

// cldoc:begin-category(Brushless)

enum CommutationType {
  BLOCK, SINUSOIDAL, TRAPEZOIDAL
};

enum ControlMode {
  VOLTAGE_CONTROL, POSITION_CONTROL, CURRENT_CONTROL, DEBUGGING
};

class Brushless {
public:
  // Has to be defined for each driver / feedback type combo
  // virtual void enable(bool flag) = 0;
  // Should use waveform from base class
  virtual void setMotorPhases(float electricalAngle, float amplitude, CommutationType waveform) = 0;
  virtual void setMotorPhases(LogicValue e1, float p1, LogicValue e2, float p2, LogicValue e3, float p3) = 0;

  ControlMode mode;
  CommutationType waveform;
  Encoder encoder;
  volatile float amplitude = 0;
  volatile float pos_act_01 = 0;
  volatile int commutationTime = 0;

  // Call after initialzing pins
  void setMotorProps(int polePairs);
  void setCurrentPins(uint8_t is1, uint8_t is2, uint8_t is3);

  void init(uint32_t freq, float pwmPos);
  void commutate();
  int16_t readCurrent();
  void dumpCurrentCalibrate(uint32_t duration, float elang);
  void calibrate();
  void update(float pwmInput);

  void openLoopTest(uint32_t pause);

  // Modified from Piccoli
  void calcSpaceVector(float electricalAngle, float amplitude, uint8_t *enables, float *pwms, CommutationType commutation);
protected:
  DLPF velF;
  int POLE_PAIRS;
  // This should be in EEPROM
  // TODO: write a initialization part that uses Serial to ask for this and any other motor-specific parameters?
  int pos_zer=0;
  float posctrl0 = 0, motorVel = 0, posDes = 0;
  float velInt = 0;
  bool motorEnableFlag = false;
  int countsPerElecRev = 0; // calculated
  volatile int pos_act=0;
  volatile uint8_t section = 0;
  uint16_t currents[3];
  volatile int16_t current = 0;
  uint8_t is1, is2, is3;

  float posToRadians(uint32_t pos);
};

// class BrushlessHighLow : public Brushless {
// public:

// protected:
//   const uint8_t h1, h2, h3, l1, l2, l3;
// };

class BrushlessPwmEn : public Brushless {
public:
  // From base class
  // void enable(bool flag);
  void setMotorPhases(float electricalAngle, float amplitude, CommutationType waveform);
  void setMotorPhases(LogicValue e1, float p1, LogicValue e2, float p2, LogicValue e3, float p3);

  BrushlessPwmEn(uint8_t pwm1, uint8_t pwm2, uint8_t pwm3, uint8_t en1, uint8_t en2, uint8_t en3) : pwm1(pwm1), pwm2(pwm2), pwm3(pwm3), en1(en1), en2(en2), en3(en3) {}
protected:
  const uint8_t pwm1, pwm2, pwm3, en1, en2, en3;
};


// cldoc:end-category()

#endif

#endif
