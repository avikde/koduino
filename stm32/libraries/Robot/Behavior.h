#ifndef Behavior_h
#define Behavior_h

class Behavior {
public:
  virtual void begin() = 0;
  virtual void update() = 0;
  virtual bool running() = 0;
  virtual void end() = 0;
  virtual void signal() {}
};

#endif