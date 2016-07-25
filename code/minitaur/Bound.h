#ifndef Bound_h
#define Bound_h

#include "VirtualLeg.h"
#include <Behavior.h>

extern bool autopilot;

class BoundLeg : public LegPair {
public:
  using LegPair::LegPair;//inherit constructor
  // BOUND
  void update();
};

class Bound : public Behavior {
public:
  BoundLeg front, rear;

  Bound() : front(BoundLeg(0, 2)), rear(BoundLeg(1, 3)) {}
  // From base class
  void begin();
  void update() {
    front.update();
    rear.update();
    // Position reading errors can cause catastrophic large changes in xd
    float newSpeed = 0.5 * (front.speed + rear.speed);
    if (fabsf(newSpeed - X.xd) < 0.2)
      X.xd = newSpeed;
    X.mode = (uint8_t)front.mode + (uint8_t)((rear.mode)<<2);
  }
  bool running() {
    return !(front.mode == STAND && rear.mode == STAND);
  }
  void end() {
    front.end();
    rear.end();
  }
  void signal() {
    //
  }
};
extern Bound bound;

#endif