#ifndef Board_h
#define Board_h

#include <Arduino.h>

class Board {
public:
  void errorStop(const char *msg);

  // led, all the other extern'd things should be members
  uint8_t led;
};

#endif
