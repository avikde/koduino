
#include <Motor.h>
#if REMOTE_TYPE==REMOTE_NUNCHUCK
#include <Wire.h>
#include <Nunchuck.h>
#endif
#if USE_BUS
#include <DxlNode.h>
#include <DxlMotor.h>
#endif
#include <SPI.h>
#if IMU_TYPE==IMU_MPU6000
#include <MPU6000.h>
#include <Eigen.h>
#endif
#if IMU_TYPE==IMU_VN100
#include <VN100.h>
#endif
// #include <OpenLog.h>
#include <BulkSerial.h>
#include <Behavior.h>
#include "HAL.h"
#include "Bound.h"

volatile uint32_t controlTime = 0;
// remote
volatile bool running, throttle;

Behavior *behavior = &bound;
DLPF speedDesF, yawDesF;
#if REMOTE_TYPE==REMOTE_NUNCHUCK
// need debounce for Z
uint32_t lastZPress = 0, lastCPress = 0;
#elif REMOTE_TYPE==REMOTE_COMPUTER
volatile uint32_t lastCompPacket = 0;
bool firstCompPacket = false;
struct ComputerPacket {
  uint8_t test;
  uint8_t bound;
  float speedDes;
  float yawDes;
} __attribute__ ((packed));
ComputerPacket computerPacket;
#endif

void debug() {
  Serial1 << X.t << "\t";
  Serial1 << controlTime << "\t";

  // IMU
  // Serial1 << X.yaw << "\t" << X.roll << "\t" << X.pitch << "\t";
  // Serial1 << rolldot << "\t" << pitchdot << "\t";

  // NUNCHUCK
  // Serial1 << nunchuck.isPaired << "\t" << nunchuck.buttonC << "\t" << nunchuck.buttonZ << "\t" << nunchuck.joy[0] << "\t" << nunchuck.joy[1];

  // for (int i = 0; i < 6; ++i)
  //   Serial1 << _HEX(nunchuck.data[i]) << "\t";
  // // Serial1 << !(nunchuck.data[5] & 0B00000001) << "\t" << !((nunchuck.data[5] & 0B00000010) >> 1);
  // Serial1 << nunchuck.buttonC << "\t" << nunchuck.buttonZ;

 //  // RC RECEIVER
 // for (int i=0; i<NRECPINS; ++i)
 //   Serial1 << rcCmd[i] << "\t";
 //  Serial1 << speedDes << "\t" << yawDes;

  // // MOTORS
  for (int i=0; i<NMOT; ++i) {
   // Serial1 << M[i].id << "\t";
    // Serial1 << M[i].failures << "\t";
    Serial1 << _FLOAT(M[i].getPosition(),3) << "\t";
   // Serial1 << M[i].getTorque() << "\t";
    // Serial1 << X.cur[i] << "\t";
    // int period=0, pw=0;
    // pwmInRaw(M[i].inPin, &period, &pw);
    // Serial1 << period << "\t" << pw << "\t";
  }

  // LEG
  // for (int i=0; i<4; ++i) {
  //   // Serial1 << "[" << leg[i].getPosition(EXTENSION) << ","  << leg[i].getPosition(ANGLE) << "]\t";
  //   float ur, uth;
  //   leg[i].getToeForce(ur, uth);
  //   if (i==2 || i==3) ur = -ur;
  //   Serial1 << ur << "\t";
  // }
  // Serial1 << X.xd << "\t" << speedDes << "\t" << yawDes;

  Serial1 << "\n";
  // Serial1.flush();
}

void controlLoop() {
  uint32_t tic = micros();

  halUpdate();
  controlTime = micros() - tic;

  // BEHAVIOR
  behavior->update();

  // CONTROL --------------------------------------
  // TEST individual motors
  // for (int i=0; i<NMOT; ++i) {
  //   M[i].enable(true);
  //   M[i].setGain(0.5);
  //   M[i].setPosition(1.5);
  //   // M[i].setOpenLoop(0.1);
  // }


#if REMOTE_TYPE==REMOTE_RC
  // REMOTE STUFF
  // this depends on the position of the "throttle", so setting that before flipping the switch could select different behaviors?
  throttle = (rcCmd[1] > 6.5 && rcCmd[1] < 15);

  const float remote0 = 7.87;//8.75
  float rvstick = (rcCmd[0] - remote0);
  float rhstick = (rcCmd[3] - remote0);
  float lhstick = (rcCmd[2] - remote0);
  // HIGH SENSITIVITY
  // speedDes = speedDesF.update(0.5 * rvstick);
  // yawDes = yawDesF.update(0.15 * rhstick);
  // LOW SENSITIVITY
  speedDes = speedDesF.update(0.2 * rvstick);
  yawDes = yawDesF.update(0.1 * rhstick);
  latDes = lhstick;
  if (behavior->running() && !throttle) {
    behavior->end();
    digitalWrite(led1, HIGH);
    openLog.enable(false);
  }
#elif REMOTE_TYPE==REMOTE_COMPUTER
  if (Serial1.available() >= 10) {
    uint8_t *computerBuf = (uint8_t *)&computerPacket;
    for (int i=0; i<10; ++i) {
      computerBuf[i] = Serial1.read();
    }
    if (computerPacket.test == 123) {
      // valid
      if (!firstCompPacket) {
        enable(true);
        firstCompPacket = true;
      }
      // set speedDes etc. here
      speedDes = computerPacket.speedDes;
      yawDes = computerPacket.yawDes;
      lastCompPacket = millis();
    }
  }
  // end behavior
  if (behavior->running() && computerPacket.bound == 0) {
    behavior->end();
    digitalWrite(led1, HIGH);
    openLog.enable(false);
  }
#endif
}

void setup() {
  halInit();
  // Low pass user desired speed
  speedDesF.init(0.99, CONTROL_RATE, DLPF_SMOOTH);
  yawDesF.init(0.99, CONTROL_RATE, DLPF_SMOOTH);

  // Should use 500Hz when 4 motors/port, try 1000 with 2/port
  attachTimerInterrupt(0, controlLoop, CONTROL_RATE);
  attachTimerInterrupt(1, debug, 20);

#if REMOTE_TYPE != REMOTE_COMPUTER
  enable(true);
  // for testing
  autopilot = false;
#endif
  // // TTEST bound
  // delay(10000);
  // behavior->begin();
  // digitalWrite(led1, LOW);
  // delay(5000);
  // behavior->end();
  // digitalWrite(led1, HIGH);
}

void loop() {
#if REMOTE_TYPE==REMOTE_NUNCHUCK
  nunchuck.update();
  if (nunchuck.isPaired) {
    // begin/end
    if (nunchuck.buttonC && !behavior->running()) {
      openLog.enable(true);
      behavior->begin();
      digitalWrite(led1, LOW);
      lastCPress = millis();
    }
    if (nunchuck.buttonZ && millis() - lastZPress > 500) {
      if (behavior->running()) {
        openLog.enable(false);
        behavior->end();
        digitalWrite(led1, HIGH);
      } else {
        // // CYCLE THROUGH BEHAVIORS
        // curBehavior = (curBehavior+1)%NUM_BEHAVIORS;
        // for (uint8_t i=0; i<curBehavior+1; ++i) {
        //   digitalWrite(led, LOW);
        //   delay(50);
        //   digitalWrite(led, HIGH);
        //   delay(100);
        // }
        // behavior = behaviorArray[curBehavior];
      }
      lastZPress = millis();
    }

    // turning
    yawDes = -nunchuck.joy[0];

    // speed
    // secret high scale if C button held down, tame settings otherwise
    // float speedScale = (nunchuck.buttonC && behavior->running()) ? 2.5 : 0.25;

    // Fixed speedScale, C button for jump
    float speedScale = 1.2;
    // float speedScale = 0.25;
    if (behavior->running() && nunchuck.buttonC && millis() - lastCPress > 1000) {
      behavior->signal();
      lastCPress = millis();
    }

    speedDes = speedDesF.update(nunchuck.joy[1] * speedScale);
  }
#elif REMOTE_TYPE==REMOTE_RC
  // IF REMOTE CONNECTED
  if (!behavior->running() && throttle) {
    digitalWrite(led1, LOW);
    behavior->begin();
    openLog.enable(true);
  }
#elif REMOTE_TYPE==REMOTE_COMPUTER
  // start behavior here
  if (!behavior->running() && computerPacket.bound == 1) {
    digitalWrite(led1, LOW);
    behavior->begin();
    openLog.enable(true);
  }
#endif
}
