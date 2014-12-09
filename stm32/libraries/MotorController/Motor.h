#ifndef Motor_h
#define Motor_h

#include <Arduino.h>

// ===============================================================================
// New types
// ===============================================================================

enum MotorControlMode {
  OPEN_LOOP_MODE, POSITION_MODE
};


// cldoc:begin-category(MotorController)

// ===============================================================================
// Helper class to implement software barrier
// ===============================================================================

class Barrier {
public:
  void init() {
    enabled = false;
    ll = ul = 0;
  }

  // TODO: implement some hysteresis to prevent jittering
  float calculate(float pos) {
    if (!enabled)
      return 0;

    const static float maxVal = 1.0;

    // FIXME: differences on a circle?? can't just use >, <
    float barrier = - (1.0/(ul - pos) - 1.0/(pos - ll));
    if (pos > ul)
      barrier = -maxVal;
    if (pos < ll)
      barrier = maxVal;
    return barrier;
  }

  // members
  bool enabled;
  float ll, ul;
};

// ===============================================================================
// Base class
// ===============================================================================

class Motor {
public:
  // Has to be defined for each motor driver / feedback type combo
  virtual void enable(bool flag) = 0;
  virtual float getRawPosition() = 0;
  virtual void sendOpenLoop(float val) = 0;

  // Initialization
  void init(float zero, int8_t direction, float gearRatio);
  void setBarrier(float ll, float ul);
  // Get*() functions
  float getPosition();
  float getVelocity();
  float getOpenLoop();
  // Set motor move commands (but they are actually sent by motorUpdate())
  void setOpenLoop(float val);
  void setGain(float Kp, float Kd);
  void setGain(float Kp) { setGain(Kp, 0); }
  void setPosition(float setpoint);
  // Map output command based on direction, driverDirection
  float mapVal(float val);
  // This should be called at a more or less fixed rate (once per iteration)
  float update();

  // Static params - for every instance
  static int updateRate;
  static float velSmooth;

  float gearRatio, unwrapOffset, prevPosition;
protected:
  MotorControlMode mode;
  // Properties
  float zero;
  // +/-1. relates what the user wants to be forward with the encoder
  int8_t direction;
  // +/-1. Relates encoder forward with LOW dirPin or +ve PWM drive signal
  int8_t driverDirection;
  // Reduction ratio (AFTER the encoder), i.e. > 1 if there is a reduction. The motor must be started near 0.
  // Command
  float val, correctedVal, setpoint;
  // PD controller
  PD pd;
  // Barrier
  Barrier barrier;
  //
  bool enableFlag;
};

// ===============================================================================
// Derived classes: driver / feedback device specific
// ===============================================================================

class BlCon34 : public Motor {
public:
  // Constructor (sets defaults)
  BlCon34() : usePwmIn(true) {}

  // "Constructors"
  void init(const TimerPin& outPin_, const PinName& inPin_, float zero, int8_t dir, float gearRatio);
  void init(const TimerPin& outPin_, const PinName& inPin_, float zero, int8_t dir) {
    init(outPin_, inPin_, zero, dir, 1.0);
  }
  void init(const TimerPin& outPin_, const TimerPin& inPin_, float zero, int8_t dir, float gearRatio);
  void init(const TimerPin& outPin_, const TimerPin& inPin_, float zero, int8_t dir) {
    init(outPin_, inPin_, zero, dir, 1.0);
  }
  // From base class
  void enable(bool flag);
  float getRawPosition();
  void sendOpenLoop(float val);

protected:
  void initCommon(const TimerPin& outPin_, float zero, int8_t dir, float gearRatio);
  
  PinName outPin, inPin;
  bool usePwmIn;
};

// cldoc:end-category()



// class BlCon3 : public Motor {
// public:
//   // "Constructors"
//   void init(PinName pwmPin, PinName dPin, PinName inPin) {
//     this->pwmPin = pwmPin;
//     this->dPin = dPin;
//     this->inPin = inPin;
//     analogWrite(outPin, 0.5);
//     driverDirection = 1;
//   }

//   // From base class
//   void enable(bool flag) {
//     digitalWrite(dPin, (flag) ? HIGH : LOW);
//   }
//   float getRawPosition() {
//     return (pulseIn(inPin) * TWO_PI);
//   }
//   void sendOpenLoop(float val) {
//     analogWrite(pwmPin, map(val, -1, 1, 0.1, 0.9));
//   }

// protected:
//   PinName pwmPin, dPin, inPin;
// };


// class PololuHP : public Motor {
// public:
//   // "Constructors"
//   void init(PinName pwmPin, PinName dirPin, PinName rstPin, PinName inPin) {
//     this->pwmPin = pwmPin;
//     this->dPin = dPin;
//     this->inPin = inPin;
//     analogWrite(outPin, 0.5);
//     driverDirection = 1;
//   }

//   // From base class
//   void enable(bool flag) {
//     digitalWrite(dPin, (flag) ? HIGH : LOW);
//   }
//   float getRawPosition() {
//     return (pulseIn(inPin) * TWO_PI);
//   }
//   void sendOpenLoop(float val) {
//     analogWrite(pwmPin, map(val, -1, 1, 0.1, 0.9));
//   }

// protected:
//   PinName pwmPin, dirPin, rstPin;
// };

#endif
