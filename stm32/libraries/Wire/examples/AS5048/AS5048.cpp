
#include <Wire.h>

const PinName led = PC13;
const uint8_t kENC = 0x40;

void setup() {
  pinMode(led, OUTPUT);
  Serial1.begin(115200);

  Wire.begin();
  while(!Wire.ready(kENC));
}

void loop() {
  uint16_t magic;

  uint32_t tic = micros();
  uint8_t l = Wire.read(kENC, 255) & 0x3f;
  uint8_t h = Wire.read(kENC, 254);
  pos = ((uint16_t)h<<6) + (uint16_t)l;
  tic = micros() - tic;

  Serial1 << "POSITION = " << pos << ", took " << tic << "us.\n";

  digitalWrite(led, TOGGLE);
  delay(1000);
}

