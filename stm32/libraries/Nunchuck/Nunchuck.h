#ifndef Nunchuck_h
#define Nunchuck_h

#include <Arduino.h>
#include <Wire.h>


class Nunchuck {
public:
  uint8_t data[6];
  // data for public access
  bool isPaired;
  bool buttonZ, buttonC;
  float joy[2];
  float accel[3];
  float roll, pitch;

  // calibration params are initialized in constructor, but can be modified
  int accelZeros[3];
  uint8_t joyZeros[2];

  // Constructor
  Nunchuck();

  // Functions that communicate with the Nunchuck
  // pair pin not connected: try to guess when paired from the kind of garbage
  void begin(TwoWire *tw);
  // pair pin connected
  void begin(TwoWire *tw, uint8_t pairPin);
  void update();

protected:
  TwoWire *tw;
  bool hasPairPin;
  uint8_t pairPin;

  void sendInit();
};

#endif
