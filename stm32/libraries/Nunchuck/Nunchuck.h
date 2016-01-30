/**
 * @authors Avik De <avikde@gmail.com>, adapted from WiiChuck library by Tim Hirzel http://www.growdown.com

  This file is part of koduino <https://github.com/avikde/koduino>

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation, either
  version 3 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, see <http://www.gnu.org/licenses/>.
 */
#ifndef Nunchuck_h
#define Nunchuck_h

#include <Arduino.h>
#include <Wire.h>

/**
 * @brief Nunchuck class
 * @sa TwoWire
 */
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
  bool initSent;

  void sendInit();
};

#endif
