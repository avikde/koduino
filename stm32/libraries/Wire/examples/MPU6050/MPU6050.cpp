
#include <Wire.h>

const PinName led = PC13;
const uint8_t kMPU = 0x68;

void setup() {
  pinMode(led, OUTPUT);
  Serial1.begin(115200);

  Wire.begin();
  while(!Wire.ready(kMPU));
}

void loop() {
  uint8_t magic;

  uint32_t tic = micros();
  magic = (Wire.read(kMPU, 0x75) & 0b01111110) >> 1;
  tic = micros() - tic;

  Serial1 << "WHO_AM_I = 0x" << _HEX(magic) << ", took " << tic << "us.\n";

  digitalWrite(led, TOGGLE);
  delay(1000);
}

