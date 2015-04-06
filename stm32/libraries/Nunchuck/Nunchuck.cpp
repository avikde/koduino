
#include "Nunchuck.h"

Nunchuck::Nunchuck() {
  accelZeros[0] = 510;
  accelZeros[1] = 490;
  accelZeros[2] = 460;

  joyZeros[0] = 124;
  joyZeros[1] = 132;

  isPaired = false;
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
}

void Nunchuck::update() {
  static const float ONE_OVER_128 = 1/(float)128;
  static const float RADIUS = 210.0;

  tw->requestFrom(0x52, 6); // request data from nunchuck
  int cnt = 0;
  while (tw->available()) {
    // receive byte as an integer
    data[cnt] = tw->read(); //
    cnt++;
  }

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

