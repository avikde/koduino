@addtogroup Wire

### Usage

1. Call pinMode() to set the pin to `OUTPUT`, `OUTPUT_OPEN_DRAIN`, `INPUT`, `INPUT_PULLUP`, or `INPUT_PULLDOWN`
2. Call digitalWrite() with `HIGH`, `LOW` or `TOGGLE` on `OUTPUT` pins
3. Call digitalRead() on `INPUT` pins

### Example: MPU6050

~~~{.cpp}

#include <Wire.h>

const int led = PC13;
const uint8_t kMPU = 0x68;

void setup() {
  pinMode(led, OUTPUT);
  Serial1.begin(115200);

  Wire.setSpeed(WIRE_1M);
  Wire.begin();
}

void loop() {
  uint8_t magic;

  uint32_t tic = micros();

  Wire.beginTransmission(kMPU);
  Wire.write(0x75);
  Wire.endTransmission(false);
  Wire.requestFrom(kMPU, 1);
  magic = Wire.read();

  tic = micros() - tic;

  Serial1 << "WHO_AM_I = 0x" << _HEX(magic) << ", took " << tic << "us.\n";

  digitalWrite(led, TOGGLE);
  delay(1000);
}

~~~

### Example: Nunchuck

~~~{.cpp}

#include <Wire.h>

const int led = PC13;

void setup() {
  pinMode(led, OUTPUT);
  Serial1.begin(115200);

  Wire.begin();

  Wire.beginTransmission(0x52);       // device address
  Wire.write(0xF0);
  Wire.write(0x55);
  Wire.endTransmission();
  delay(1);
  Wire.beginTransmission(0x52);
  Wire.write(0xFB);
  Wire.write(0x00);
  Wire.endTransmission();
}

uint8_t data[6];

void loop() {
  uint32_t tic = micros();

  Wire.requestFrom(0x52, 6); // request data from nunchuck
  int cnt = 0;
  while (Wire.available()) {
    // receive byte as an integer
    data[cnt] = Wire.read(); //
    cnt++;
  }
  tic = micros() - tic;

  Serial1 << tic << "\t";
  for (int i = 0; i < 6; ++i)
    Serial1 << _HEX(data[i]) << "\t";
  Serial1 << !(data[5] & 0B00000001) << "\t" << !((data[5] & 0B00000010) >> 1);
  Serial1 << "\n";

  digitalWrite(led, TOGGLE);
  delay(100);

  Wire.beginTransmission(0x52);      // transmit to device 0x52
  Wire.write(0x00);         // sends one byte
  Wire.endTransmission(false);    // stop transmitting
}

~~~

