/**
 * @authors Avik De <avikde@gmail.com>

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
