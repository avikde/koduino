@addtogroup Wire

### Usage

1. (Optionally) call `setSpeed()`
2. Call `begin()`
3. See below for reading/writing

#### Writing a register

1. Call beginTransmission() with the 7-bit slave address
2. Call write() as many times as bytes to write
3. Call endTransmission() (without an argument to send a STOP--recommended).

#### Reading a register

One of the most common I2C operations is to read a register from a slave (e.g. MPU6050 and AS5048 examples below). The Arduino API makes this a bit tedious, but the "official" way to do this (fully compatible with Arduino) is to:

1. Call beginTransmission() with the 7-bit slave address
2. write() the register name
3. Call endTransmission() with the argument `false` (don't sent STOP)
4. Call requestFrom() with the 7-bit slave address and a length of 1 (this sends STOP)
5. Call read() to get the register value

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

### Example: AS5048B

~~~{.cpp}

#include <Wire.h>

const int led = PC13;
const uint8_t kAMS = 0x40;

void setup() {
  pinMode(led, OUTPUT);
  Serial1.begin(115200);

  // Try increasing speed if it works
  // Wire.setSpeed(WIRE_1M);
  Wire.begin();
}

void loop() {
  uint32_t tic = micros();

  Wire.beginTransmission(kAMS);
  Wire.write(255);
  Wire.endTransmission(false);
  Wire.requestFrom(kAMS, 1);
  uint8_t l = Wire.read() & 0x3f;

  Wire.beginTransmission(kAMS);
  Wire.write(254);
  Wire.endTransmission(false);
  Wire.requestFrom(kAMS, 1);
  uint8_t h = Wire.read();

  tic = micros() - tic;

  Serial1 << "Read position = " << ((uint16_t)h<<6) + (uint16_t)l << ", took " << tic << "us.\n";

  digitalWrite(led, TOGGLE);
  delay(50);
}

~~~

