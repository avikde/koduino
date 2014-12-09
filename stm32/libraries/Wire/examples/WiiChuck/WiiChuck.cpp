
#include <Wire.h>

const PinName led = PC13;
uint32_t tic, toc;

class Chuck {
public:
  // variables for access
  float joyX, joyY;
  bool zPressed, cPressed;
  uint8_t status[6];

  // methods
  Chuck() : NUNCHUCK_ADDRESS(0x52) {}

  void begin() {
    Wire.setSpeed(WIRE_100K);
    Wire.begin();
    while(!Wire.ready(NUNCHUCK_ADDRESS));

    // Wire.beginTransmission(NUNCHUCK_ADDRESS);
    // Wire.write(0xF0);
    // Wire.write(0x55);
    // Wire.endTransmission();
    // delay(1);
    // Wire.beginTransmission(NUNCHUCK_ADDRESS);
    // Wire.write(0xFB);
    // Wire.write(0x00);
    // Wire.endTransmission();
    // delay(1);
    Wire.write(NUNCHUCK_ADDRESS, 0xF0, 0x55);
    delay(1);
    Wire.write(NUNCHUCK_ADDRESS, 0xFB, 0x00);
    delay(1);
    Wire.read(NUNCHUCK_ADDRESS, 5, status);
  }

  void update() {
    tic = micros();
    // Wire.beginTransmission(NUNCHUCK_ADDRESS);
    // Wire.write(0x00);
    // Wire.endTransmission();
    // Wire.requestFrom(NUNCHUCK_ADDRESS, 5);
    Wire.read(NUNCHUCK_ADDRESS, 0, 5, status);
    toc = micros() - tic;
    // Wire.beginTransmission(NUNCHUCK_ADDRESS);
    // Wire.write((uint8_t)0x00);
    // Wire.endTransmission();
    // Wire.requestFrom(NUNCHUCK_ADDRESS, 6);
    // int cnt = 0;
    // while (Wire.available()) {
    //   // receive byte as an integer
    //   status[cnt] = Wire.read();
    //   cnt++;
    // }

    // Joystick
    joyX = (status[0] - 0x80)/((float)0x80);
    joyY = (status[1] - 0x80)/((float)0x80);

    // FIXME byte 5!!!

    // Accelerometer
    // for (int i = 0; i < 3; i++)
    //   angles[i] = (status[i+2] << 2) + ((status[5] & (B00000011 << ((i+1)*2) ) >> ((i+1)*2)));

    // Buttons
    bool buttonZ = !( status[5] & 0b00000001);
    bool buttonC = !((status[5] & 0b00000010) >> 1);
    lastZ = buttonZ;
    lastC = buttonC;
    cPressed = (buttonC && !lastC);
    zPressed = (buttonZ && !lastZ);
  }
protected:
  const uint8_t NUNCHUCK_ADDRESS;
  bool lastZ, lastC;
};

Chuck chuck;

void setup() {
  digitalWrite(led, HIGH);
  pinMode(led, OUTPUT);
  Serial1.begin(115200);

  chuck.begin();
}

void loop() {
  // uint32_t tic = micros();
  chuck.update();
  // uint32_t toc = micros() - tic;

  Serial1 << toc << "\t";
  for (int i=0; i<6; ++i)
    Serial1 << _HEX(chuck.status[i]) << "\t";
  // Serial1 << toc << "\tX=" << chuck.joyX << "\tY=" << chuck.joyY << "\t";
  // Serial1 << "Z=" << chuck.zPressed << "\tC=" << chuck.cPressed << "\t";

  Serial1 << "\n";

  // Logic?
  if (chuck.joyY < -0.5)
    digitalWrite(led, LOW);
  if (chuck.joyY > 0.5)
    digitalWrite(led, HIGH);

  // digitalWrite(led, TOGGLE);
  delay(50);
}

