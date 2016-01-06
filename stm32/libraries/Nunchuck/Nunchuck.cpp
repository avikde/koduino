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
#include "Nunchuck.h"

Nunchuck::Nunchuck() {
  accelZeros[0] = 510;
  accelZeros[1] = 490;
  accelZeros[2] = 460;

  joyZeros[0] = 124;
  joyZeros[1] = 132;

  isPaired = false;
  hasPairPin = false;
  initSent = false;
}

void Nunchuck::begin(TwoWire *tw, uint8_t pairPin) {
  pinMode(pairPin, INPUT_PULLDOWN);
  hasPairPin = true;
  this->pairPin = pairPin;
  begin(tw);
}

void Nunchuck::begin(TwoWire *tw) {
  this->tw = tw;
  tw->setSpeed(WIRE_100K);
  tw->begin();

  sendInit();
}

void Nunchuck::sendInit() {
  tw->beginTransmission(0x52);       // device address
  tw->write(0xF0);
  tw->write(0x55);
  tw->endTransmission();
  delay(1);
  tw->beginTransmission(0x52);
  tw->write(0xFB);
  tw->write(0x00);
  tw->endTransmission();
  // done
  initSent = true;
}

void Nunchuck::update() {
  static const float ONE_OVER_128 = 1/(float)128;
  static const float RADIUS = 210.0;

  if (hasPairPin) {
    isPaired = (digitalRead(pairPin) == HIGH);
    if (!isPaired)
      return;
    // paired, send init?
    if (!initSent) {
      sendInit();
      return;
    }
  }

  tw->requestFrom(0x52, 6); // request data from nunchuck
  int cnt = 0;
  while (tw->available()) {
    // receive byte as an integer
    data[cnt] = tw->read(); //
    cnt++;
  }

  if (!hasPairPin) {
    // try to guess pairing based on type of garbage
    if (data[0] == 0 && data[1] == 0) {
      // not paired
      isPaired = false;
      return;
    }

    if (data[0] == 0xff && data[1] == 0xff) {
      // someone synced but need to re-init
      isPaired = false;
      sendInit();
      return;
    }

    // data is probably OK
    isPaired = true;
  }

  // Process the data
  buttonZ = !(data[5] & 0B00000001);
  buttonC = !((data[5] & 0B00000010) >> 1);

  for (uint8_t ax=0; ax<2; ++ax) {
    joy[ax] = (data[ax] - joyZeros[ax]) * ONE_OVER_128;
  }

  int angles[3];
  for (uint8_t i=0; i<3; ++i) {
    angles[i] = (data[i+2] << 2) + ((data[5] & (0B00000011 << ((i+1)*2)) >> ((i+1)*2)));

    accel[i] = (float)(angles[i] - accelZeros[i]);
  }
  roll = atan2f(accel[0], accel[2]);
  pitch = acosf(accel[1]/RADIUS);

  // Request more data
  tw->beginTransmission(0x52);      // transmit to device 0x52
  tw->write(0x00);         // sends one byte
  tw->endTransmission(false);    // stop transmitting
}

