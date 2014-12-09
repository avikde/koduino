
#include "Motor.h"

// ===============================================================================
// Base class
// ===============================================================================

// Static members - can be overridden
int Motor::updateRate = 1000;
float Motor::velSmooth = 0.8;

void Motor::init(float zero, int8_t direction, float gearRatio) {
  this->zero = zero;
  // check
  if (direction != -1 && direction != 1)
    direction = 1;
  this->direction = direction;

  this->gearRatio = gearRatio;
  this->unwrapOffset = 0;
  this->prevPosition = NAN;

  mode = OPEN_LOOP_MODE;
  val = correctedVal = setpoint = 0;
  driverDirection = 1;
  barrier.init();
  enableFlag = false; // Only matters for BLCon v3.4
  pd.init(Motor::velSmooth, Motor::updateRate, DLPF_ANGRATE);
}

void Motor::setBarrier(float ll, float ul) {
  barrier.ll = ll;
  barrier.ul = ul;
  barrier.enabled = true;
}

float Motor::getPosition() {
  // FIXME: the "with gear ratio" could should work in general but there's a bug
  // without gear ratio:
  if (abs(gearRatio-1) < 0.0001)
    return fmodf_mpi_pi(getRawPosition() * direction - zero);
  
  // with gear ratio
  // need previous position
  static float curPos;
  curPos = fmodf_mpi_pi(getRawPosition());
  if (!isnan(prevPosition)) {
    if (curPos - prevPosition < -PI)
      unwrapOffset += TWO_PI;
    else if (curPos - prevPosition > PI)
      unwrapOffset -= TWO_PI;
  }
  prevPosition = curPos;
  return (unwrapOffset + curPos) * direction / gearRatio - zero;
}

float Motor::getVelocity() {
  return pd.vel;
}

float Motor::getOpenLoop() {
  return val;
}

void Motor::setGain(float Kp, float Kd) {
  pd.setGain(Kp, Kd);
}

void Motor::setOpenLoop(float val) {
  this->mode = OPEN_LOOP_MODE;
  this->val = val;
}

void Motor::setPosition(float setpoint) {
  this->mode = POSITION_MODE;
  this->setpoint = setpoint;
}

float Motor::mapVal(float val) {
  return constrain(val * direction * driverDirection, -1, 1);
}

float Motor::update() {
  float pos = getPosition();

  // Velocity calculation should happen independent of mode
  float error = fmodf_mpi_pi(pos - setpoint);
  float posCtrlVal = pd.update(error);

  // In position mode, update the motor command
  if (mode == POSITION_MODE)
    val = posCtrlVal;
  // In open-loop mode, val has been set and nothing else needs to be done

  // Send command, but don't modify "val" (set by user)
  correctedVal = mapVal(val + barrier.calculate(pos));

  // send the command
  sendOpenLoop(correctedVal);

  // Return the command so that the slave can do the same thing
  return correctedVal;
}

// ===============================================================================
// Derived classes: driver / feedback device specific
// ===============================================================================

void BlCon34::initCommon(const TimerPin& outPin_, float zero, int8_t dir, float gearRatio) {
  pinMode(outPin_, PWM);
  this->outPin = outPin_.pin;
  analogWrite(outPin, 0);
  driverDirection = 1;
  // Call base class init
  Motor::init(zero, dir, gearRatio);
}

void BlCon34::init(const TimerPin& outPin_, const PinName& inPin_, float zero, int8_t dir, float gearRatio) {
  usePwmIn = true;
  pwmIn(inPin_);
  this->inPin = inPin_;
  initCommon(outPin_, zero, dir, gearRatio);
}

void BlCon34::init(const TimerPin& outPin_, const TimerPin& inPin_, float zero, int8_t dir, float gearRatio) {
  usePwmIn = false;
  // pinModePulseIn(inPin_);
  pinMode(inPin_, PULSE_IN);
  this->inPin = inPin_.pin;
  initCommon(outPin_, zero, dir, gearRatio);
}

void BlCon34::enable(bool flag) {
  enableFlag = flag;
}

float BlCon34::getRawPosition() {
  return map(usePwmIn ? pwmIn(inPin) : pulseIn(inPin), 0.1, 0.9, 0, TWO_PI);
}

void BlCon34::sendOpenLoop(float val) {
  analogWrite(outPin, (enableFlag) ? map(val, -1, 1, 0.11, 0.89) : 0);
}
