
#include "Bound.h"

Bound bound;

// TEST globals for autopilot
bool autopilot = false;
uint32_t tstart = 0;
float headingDes = 0;

void Bound::begin() {
  if (autopilot) {
    tstart = X.t;
    headingDes = X.yaw;
  }
  front.begin();
  delay(400);
  rear.begin();
}

void BoundLeg::update() {
  // useful vars
  float ext = getPosition(EXTENSION);
  float extVel = getVelocity(EXTENSION);
  bool bFront = (legi[0]==0);

  // BOUND PARAMS
  float flightPos = 1.57;
  // const float flightRetractPos = 1.0;
  // const uint32_t tretract = 70;
  float tdPos = flightPos-0.1;
  // desired standing leg angle depends on front or back
  float angDes = (bFront) ? 0.0 : 0.13;//MINITAUR
  // float angDes = (bFront) ? -0.05 : 0.2;//MINITAUR WEIGHTS
  // float angDes = (bFront) ? 0.0 : 0.2;//MEGATAUR

  // STANCE PARAMS
  int tstance = 115;//MINITAUR
  // uint32_t tstance = 300;//MEGA
  float kRoll = 0.5, kRollDot = 0.07;//MMINI
  // float kRoll = 0.7, kRollDot = 0.09;// MEGA
  // float kSpring = 1.8;//MEGA
#if USE_BUS
  float kSpring = 0.75;//MINI
  float kVert = 0.17;//MINI
#else
  // from old code
  // float kSpring = 0.55;//MINI
  // float kVert = 0.21;//MINI
  float kSpring = 0.55;//MINI
  float kVert = 0.21;//MINI
#endif
  // float kVert = 0.4;//MEGA
  float kOffset = 0.033;//constrain(0.05 * fabsf(X.xd), 0, 0.2);
  // float kOffset = 0.1;//WEIGHTS
  // test
  // tstance = 110 - (int)(5 * fabsf(X.xd));
  // kVert = 0.17+constrain(0.05 * fabsf(X.xd), 0, 0.2);
  // kVert = 0.17+constrain(0.02 * latDes, 0, 0.05);
  float kAngStance = 0.7;//0.35;//MINI
  // float kAngStance = 1.0;//MINI WEIGHTS
  // float kAngStance = 1.0;
  // FLIGHT PARAMS
  float kExtFltP = 0.4, kExtFltD = 0.005;//MINI
  float kAngFltP = 0.3, kAngFltD = 0.005;//MINI
  // float kExtFltP = 0.8, kExtFltD = 0.0;//MEGA
  // float kAngFltP = 1.0, kAngFltD = 0.0;//MEGA

  // STAND PARAMS
  float kExtStand = 0.4;
  float kAngStand = 0.7;

  // SPEED DEPENDENT PARAMS
  // kVert += 0.05 * constrain(fabsf(X.xd), 0, 2);
  // kOffset += 0.05 * constrain(fabsf(X.xd), 0, 1);
  // tstance -= (int)(10 * constrain(fabsf(X.xd), 0, 2));

  // Half sweep angle = arcsin(xdot * Ts / (2 rho))
  // The extension @TD is the FKext(flightPos) ~= 0.17
  float kStrideLength = tstance*0.001 / 0.34;//MINI
  // float kStrideLength = tstance*0.001 / 0.69;//MEGA

  // TEST
  if (autopilot) {
    // overwrite speedDes and yawDes
    // yawDes = -0.08;//0.3 * fmodf_mpi_pi(X.yaw - headingDes) - 0.05;
    speedDes = 0;
    int t2 = X.t - tstart - 2000;
    if (t2 > 0 && t2 <= 10000) {
      speedDes = 0.0001 * t2;
    } else if (t2 > 10000) {
      speedDes = 1.0 + 0.0001 * (t2 - 10000);
    }
  }

  if (mode == STANCE) {
    float frac = interpFrac(tTD, tTD + tstance, X.t);
    // float ang = atan2((ext - tdPos)*PI/tstance, extVel);
    // float phaseTerm = constrain(0.15 * latDes, -0.3, 0.3);//-0.3;
    // TEST
    float phaseTerm = 0;//-0.3 * constrain(fabsf(X.xd), 0, 1);
    float vertTerm = -kVert * arm_cos_f32(frac * PI);
    // TEST
    // if (fabsf(X.xd) > 1)
    //   kOffset += 0.01 * (fabsf(X.xd) - 1);
    float uvert = kSpring * (flightPos - ext) + vertTerm + phaseTerm * arm_sin_f32(frac*PI) + kOffset;
    float uroll = kRoll * X.roll + kRollDot * rolldot;
    uroll = constrain(uroll, -0.2, 0.2);
    setOpenLoop(EXTENSION, uvert, uroll);

    // FA: very loose spring
    // angDes += constrain(asinf(kStrideLength * speedDes), -0.2, 0.2);//NEW
    // OLD
    angDes += asinf(kStrideLength * speedDes);
    // setGain(ANGLE, kAngStance);
    // setPosition(ANGLE, angDes, 0);
    setOpenLoop(ANGLE, kAngStance * (angDes - getPosition(ANGLE)), 0);
    
    speedAccum += getSpeed(X.pitch);

    if (X.t - tTD > tstance) {//ext > tdPos && extVel > 0) {
      mode = FLIGHT;
      tLO = X.t;
      // LP filter this
      speed = speedAccum / (0.001*tstance*CONTROL_RATE);
      speedAccum = 0;
//      tstance = t - tTD;
//      if (tstance < 50 || tstance > 150)
//        tstance = 110;
    }
  } else if (mode == FLIGHT) {
    setGain(EXTENSION, kExtFltP, kExtFltD);
    // setPosition(EXTENSION, (X.t - tLO > tretract) ? flightPos :  flightRetractPos, 0);
    setPosition(EXTENSION, flightPos, 0);

    // FOREAFT
    // get to the neutral point, servo around it
    // float stepPos = kStrideLength * (-0.5*X.xd + 0.35 * (speedDes-X.xd));
    // stepPos = constrain(stepPos, -0.2, 0.2);
    // angDes += asinf(stepPos);
    angDes += asinf(kStrideLength * (-X.xd + 0.3 * (speedDes-X.xd)));
    if (X.t - tLO < 20)
      setGain(ANGLE, 0);
    else
      setGain(ANGLE, kAngFltP, kAngFltD);
    // 
    // yawDes is between +/- 1
    // FIXME: correct for body pitch, but IMU delay
    float uyaw = - constrain(yawDes, -1, 1);
    setPosition(ANGLE, angDes, uyaw);
    
    if (ext < tdPos && extVel < 0)// && X.t - tLO > tretract+20)
    { 
      mode = STANCE;
      tTD = X.t;
    }
  } else {
    // stand
    setGain(EXTENSION, kExtStand);
    setGain(ANGLE, kAngStand);
    setPosition(ANGLE, angDes, 0);
    setPosition(EXTENSION, flightPos, 0);
    X.xd = 0;
  }
}


