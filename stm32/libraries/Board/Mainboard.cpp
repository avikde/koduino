
#include "Mainboard.h"


// Constructor
#if defined(SERIES_STM32F4xx)
Mainboard::Mainboard() : 
  J1(PA3, PB8),
  J2(PA2, PB9),
  J3(PA0, PB3),
  J4(PA1, PA8),
  J5(PB0, PA11),
  J6(PB1, PA15),
  J7(PA6, PB14),
  J8(PB5, PB15),
  J9(PB7, PB6),//I2C1,Serial1
  J10(PB10, PB11),//I2C2
  SSIMU(PA14),
  lastLogWrite(0)
{}
#elif defined(SERIES_STM32F37x)
Mainboard::Mainboard() : 
  J1(PA0, PA6),
  J2(PA11, PB5),
  J3(PA2, PB6),
  J4(PA3, PB7),
  J5(PB0, PB1),
  J6(PB8, PB9),// Also Serial3
  J7(PA14, PA15),// Also I2C1
  J8(PA1, PA5),
  SSIMU(PC15),
  logger(PB2, Serial2, 115200),
  lastLogWrite(0)
{}
#endif

void Mainboard::init(const char *header, const char *fmt, uint32_t packetSize) {
#if defined(SERIES_STM32F4xx)
  led = PC15;
  SPI.setPins(PA5, 5, PB4, 5, PA7, 5);
#elif defined(SERIES_STM32F37x)
  led = PC13;
  SPI.setPins(PA12, 6, PA13, 6, PF6, 5);
#endif

  // Turn on led while initing
  pinMode(led, OUTPUT);
  digitalWrite(led, LOW);

  // PC comms
  Serial1.begin(115200);

  // Logging
  if (packetSize > 0) {
    if (!logger.init(header, fmt, packetSize))
      errorStop("Logger init failed");
  }

  // delay for IMU?
  delay(100);

  // IMU init
  if (!mpu.init(SSIMU))
    errorStop("WHO_AM_I did not match");
  // Start off the EKF
  mpu.readSensors();
  ekf.init(mpu.acc);

  // Hardware setup done
  digitalWrite(led, HIGH);

  // NOTE: attach motors as
  // M1.init(&J1, zero, dir [, gearRatio]);
}


const EulerState * Mainboard::update(const uint8_t *pX) {
  // write to log
  if (millis() - lastLogWrite > 8) {
    logger.write(pX);
    lastLogWrite = millis();
  }
  return update();
}


const EulerState * Mainboard::update() {
  // IMU
  mpu.readSensors();
  ekf.update(mpu.acc, mpu.gyr);
  // TODO allow setting of a rotation matrix
  // X.pitch = e->angles[1];
  // X.pitchdot = e->angRates[1];
  // X.roll = e->angles[0];
  // X.rolldot = e->angRates[0];
  return ekf.getEuler();

  // STATE UPDATE done by other code
}

// Global object
Mainboard mb;

