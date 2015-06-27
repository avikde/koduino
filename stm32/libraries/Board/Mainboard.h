#ifndef Mainboard_h
#define Mainboard_h

#include <Arduino.h>
#include <MPU6000.h>
#include <EKF.h>
#include <Motor.h>
#include "Board.h"

#if defined(SERIES_STM32F37x)
#include <OpenLog.h>
#endif

class Mainboard : public Board {
public:
  // Constructor
  Mainboard();

  // Init with logger settings
  void init(const char *header, const char *fmt, uint32_t packetSize);
  // Init without logger
  void init() { init(NULL, NULL, 0); } 
  // Update returns IMU stuff
  const EulerState * update();
  // update and write to log
  const EulerState * update(const uint8_t *pX);

  const MotorConnector J1, J2, J3, J4, J5, J6, J7, J8;
#if defined(SERIES_STM32F4xx)
  const MotorConnector J9, J10;
#endif
  const uint8_t SSIMU;

  // Logger is different
#if defined(SERIES_STM32F4xx)
  // TODO SDIO
#elif defined(SERIES_STM32F37x)
  OpenLog logger;
#endif

protected:
  volatile uint32_t lastLogWrite;
  // IMU
  MPU6000 mpu;
  EKF ekf;

  void initCommon(bool useLogger);
};

extern Mainboard mb;

#endif
