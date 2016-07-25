
#include <Arduino.h>
#include <WMath.h>
#include "HAL.h"

const uint8_t led0 = PD0, led1 = PD1;

#if USE_BUS
// mblc0.4
DxlNode master9(PA12, Serial4);
DxlNode master8(PA15, Serial5);

DxlMotor M[NMOT];
const uint8_t NMOT9 = 4, NMOT8 = 4;
const uint8_t ids9[] = {0, 1, 2, 3};
const uint8_t ids8[] = {4, 5, 6, 7};
#else

const uint8_t pwmPin[] = {PE9, PE11, PE13, PE14, PA0, PD4, PD7, PD6, PB4, PB5};
const uint8_t posPin[] = {PD12, PD13, PD14, PD15, PC6, PC7, PC8, PC9, PE2, PE3};
const uint8_t curPin[] = {PD8, PB2, PA13, PB1, PB0, PD5, PF9, PF10, PE4, PE5};
const uint8_t motorPort[8] = {0, 1, 2, 3, 4, 5, 6, 7};//MINI
// const uint8_t motorPort[8] = {0, 1, 2, 3, 4, 5, 6, 8};//MEGA
BlCon34 M[NMOT];
#endif

// This must be set per robot
// MINITAUR
const float motZeros[] = {0.752, 0.810, 2.130, 5.841, 5.321, 2.865, 1.912, 4.390};
const int8_t dir[] = {1, 1, 1, 1, -1, -1, -1, -1};
// MEGATAUR
// const float motZeros[8] = {1.06, 0.36, 2.00, 0.58, 1.76, 1.00, 5.58, 2.09};
// const int8_t dir[] = {-1, -1, 1, 1, 1, 1, -1, -1};

#if USE_BUS
const int CONTROL_RATE = 400;
#else
const int CONTROL_RATE = 1000;
#endif
// heartbeat
volatile uint32_t nIters = 0;
// volatile uint32_t lastPrint = 0;
int hbFreq = CONTROL_RATE/10;
int hbOnFreq = CONTROL_RATE/20;

#if REMOTE_TYPE==REMOTE_NUNCHUCK
// Nunchuck
Nunchuck nunchuck;
#elif REMOTE_TYPE==REMOTE_RC
// RC receiver pins
const uint8_t rcRecPin[] = {PC15, PC14, PC13, PB7, PB6, PD3};
// const uint8_t rcRecPin[] = {PB7, PB6, PD3};
volatile float rcCmd[NRECPINS];
#endif//if USE_NUNCHUCK

float speedDes = 0, yawDes = 0, latDes = 0;

// bus vars
volatile uint32_t totalPings=0, totalFailures = 0;
volatile uint32_t ping = 0;//, lastPrint = 0;

#if IMU_TYPE==IMU_MPU6000
#include <MPU6000.h>
#include <EKF.h>
// IMU
MPU6000 mpu(SPI_2);
EKF ekf(0.1, 1.0, 1/((float)CONTROL_RATE));
#elif IMU_TYPE==IMU_VN100
#include <VN100.h>
VN100 vn100(SPI_2);
#endif

// OpenLog
// OpenLog openLog(PE15, Serial3, 115200);
BulkSerial openLog(MBLC_OPENLOG);
volatile uint32_t lastOLwrite = 0;

// BulkSerial computer(MBLC_RPI);
// ComputerPacket computerPacket = {0,0,0,0};

// STATE
volatile LogVector X;
volatile float rolldot = 0, pitchdot = 0;

// Legs
// MINITAUR
MinitaurLeg leg[4] = {MinitaurLeg(&M[1], &M[0]),//0
MinitaurLeg(&M[3], &M[2]), //1
MinitaurLeg(&M[4], &M[5]), //2
MinitaurLeg(&M[6], &M[7])};//3
// MEGATAUR
// MinitaurLeg leg[4] = {MinitaurLeg(&M[1], &M[0]),//0
// MinitaurLeg(&M[2], &M[3]), //1
// MinitaurLeg(&M[4], &M[5]), //2
// MinitaurLeg(&M[7], &M[6])};//3

// Safety check
volatile uint32_t legStuckTimer[4];

void enable(bool flag) {
  for (int i=0; i<4; ++i)
    leg[i].enable(flag);
}

void errorStop(const char *msg) {
  digitalWrite(led0, HIGH);
  while (1) {
    digitalWrite(led1, LOW);
    delay(50);
    digitalWrite(led1, HIGH);
    Serial1 << msg << endl;
    delay(1000);
  }
}

void halInit() {
  pinMode(led0, OUTPUT);
  pinMode(led1, OUTPUT);
  digitalWrite(led1, HIGH);


  Motor::updateRate = CONTROL_RATE;
  Motor::velSmooth = 0.55;
  MinitaurLeg::useLengths = false;
#if USE_BUS
  Serial4.irqnPriority = 2;
  Serial5.irqnPriority = 1;
  // Serial1.irqnPriority = 10;
  // Serial3.irqnPriority = 9;

  master8.init(NULL);
  master9.init(NULL);

  // init motors
  for (int i=0; i<NMOT9; ++i) {
    uint8_t id = ids9[i];
    M[id].init(&master9, id, motZeros[id], (id<4) ? 1 : -1);
  }
  for (int i=0; i<NMOT8; ++i) {
    uint8_t id = ids8[i];
    M[id].init(&master8, id, motZeros[id], (id<4) ? 1 : -1);
  }
  delay(1000);
  // Establish comms
  // // Need to call update on motors at least once and then resetOffset()
  // int i=0;
  // while (i < NMOT) {
  //   M[i].update();
  //   if (M[i].updated())
  //     i++;
  //   else {
  //     Serial1 << "ERROR: bus communication failed with " << i << "\n";
  //   }
  //   digitalWrite(led0, TOGGLE);
  //   digitalWrite(led1, TOGGLE);
  //   delay(50);
  // }
  // for (int i=0; i<NMOT; ++i)
  //   M[i].resetOffset();
  // Legs: update motors separately
  for (int i=0; i<4; ++i)
    leg[i].autoUpdate = false;
#else

  // wait a bit (??)
  delay(750);

  for (int i=0; i<NMOT; ++i) {
    uint8_t port = motorPort[i];
    M[i].init(pwmPin[port], posPin[port], motZeros[i], dir[i]);
    M[i].setTorqueEstParams(0.0954, 0.186, 16);

    // current reading: these are on EXTI, and on T15, T3 (for J8 and J9)
    // try to set to lower priority
    // TIMER_IC_PRIORITY = 1;
    // pinMode(curPin[port], (port<6) ? PWM_IN_EXTI : PWM_IN);
    // TIMER_IC_PRIORITY = 0;
  }
#endif//if USE_BUS

  Serial1.begin(115200);

  // REMOTE
#if REMOTE_TYPE==REMOTE_NUNCHUCK
  nunchuck.begin(&Wire, PF6);
#elif REMOTE_TYPE==REMOTE_RC
  // RC receiver
  // TIMER_IC_PRIORITY = 3;
  for (int i=0; i<NRECPINS; ++i)
    pinMode(rcRecPin[i], PWM_IN_EXTI);
#elif REMOTE_TYPE==REMOTE_COMPUTER
  //
#endif

#if IMU_TYPE==IMU_MPU6000
  // delay for IMU
  delay(150);
  if (!mpu.init(PB12))
    errorStop("IMU comm failed");
  // Start off the EKF
  mpu.readSensors();
  ekf.init(mpu.acc);
#elif IMU_TYPE==IMU_VN100
  vn100.init(PB12);
#endif

  // Try to init openlog (don't stop if no SD card)
  openLog.begin(115200, sizeof(X), (void *)&X, 0, NULL);
  openLog.initOpenLog("t,r,p,y,q0,q1,q2,q3,q4,q5,q6,q7,u0,u1,u2,u3,u4,u5,u6,u7,xd,mo", "IffffffffffffffffffffB");

  // raspi
  // computer.begin(115200, 0, NULL, sizeof(computerPacket), (void *)&computerPacket);

  // Hardware setup done
  digitalWrite(led0, HIGH);
  digitalWrite(led1, HIGH);
}

void halUpdate() {
  // MOTORS
  for (int i=0; i<4; ++i)
    leg[i].update();
  // for PWM the leg update will update this
  // for bus, autoUpdate is false, so halUpdate() updates motors
#if USE_BUS
  // update one from each master at a time
  for (uint8_t i=0; i<4; ++i) {
    // send command
    if (i < NMOT8)
      M[ids8[i]].update();
    if (i < NMOT9)
      M[ids9[i]].update();
    // wait for response
    if (i < NMOT8)
      M[ids8[i]].update2();
    if (i < NMOT9)
      M[ids9[i]].update2();
  }
#endif//if USE_BUS

#if IMU_TYPE==IMU_MPU6000
  // IMU update
  mpu.readSensors();
  ekf.update(mpu.acc, mpu.gyr);
  const EulerState *e = ekf.getEuler();
  // put in IMU offsets here
  // convention: roll right > 0, pitch forward > 0
  X.roll = e->angles[0] + 0.03;
  X.pitch = e->angles[1];
  X.yaw = e->angles[2];
  rolldot = e->angRates[0];
  pitchdot = e->angRates[1];
  // X.yawdot = e->angRates[2];
#elif IMU_TYPE==IMU_VN100
  float yaw=0, roll=0, pitch=0, _yawdot=0, _rolldot=0, _pitchdot=0;
  vn100.get(yaw, pitch, roll, _yawdot, _pitchdot, _rolldot);
  X.yaw = yaw;
  X.roll = pitch;
  X.pitch = roll;
  rolldot = _pitchdot;
  pitchdot = _rolldot;
#endif

  // SAFETY CHECK
  for (int i=0; i<4; ++i) {
    // legs get stuck
    float ri = leg[i].getPosition(EXTENSION);
    if (ri < 0.1 || ri > 3.05) {
      legStuckTimer[i]++;
    } else {
      legStuckTimer[i] = 0;
    }
    // if stuck for 0.5 seconds, disable
    if (legStuckTimer[i] > CONTROL_RATE/2) {
      enable(false);
      hbFreq = CONTROL_RATE;
      hbOnFreq = CONTROL_RATE/10;
    }
  }


  // totalPings+=NMOT;
  // ping = micros() - tic;

#if REMOTE_TYPE==REMOTE_RC
  // RC receiver
  for (int i=0; i<NRECPINS; ++i) {
    int period=0, pulsewidth=0;
    pwmInRaw(rcRecPin[i], &period, &pulsewidth);
    // no signal if transmitter is off
    float dummy = (period > 0) ? pulsewidth*100/((float)period) : 0;
    // some basic error checking
    if (dummy > 5 && dummy < 10)
      rcCmd[i] = dummy;
  }
#endif

  // LOGGING
  X.t = millis();

  for (int i=0; i<NMOT; ++i) {
    X.q[i] = M[i].getPosition();
//     float rawCur = 0;
// #if USE_BUS
//     rawCur = M[i].getCurrent();
// #else
//     // rawCur = map(pwmIn(curPin[motorPort[i]]), 0.1, 0.9, 0, 1);
// #endif
    // convert to multiples of 0.1A? current01*3.3/(0.5e-3*40)*10 = 1650
    // bias should be about 825
    // X.cur[i] = (uint16_t)(rawCur * 1650);
    X.torque[i] = M[i].getTorque();
  }
  // mode is set elsewhere
  if (X.t - lastOLwrite > 9) {
    // openLog.write((const uint8_t *)&X);
    openLog.write();
    lastOLwrite = X.t;
  }

  // heartbeat
  nIters++;
  digitalWrite(led0, (nIters % hbFreq < hbOnFreq) ? LOW : HIGH);
}

