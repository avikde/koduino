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
#ifndef IMU_h
#define IMU_h

#include <stdint.h>
#include <math.h>
// #include "OrientationFilter.h"

/** @addtogroup IMU IMU drivers and attitude estimation
 *  @{
 */

class IMU {
public:
  // void init(OrientationFilter *filt) {
  //   this->filt = filt;
  //   acc[0] = acc[1] = acc[2] = 0;
  //   gyr[0] = gyr[1] = gyr[2] = 0;
  // }

  // This function must be implemented by each IMU hardware; must set acc,gyr.
  virtual void readSensors() = 0;

  // const EulerState * update() {
  //   readSensors();
  //   // filt->update(acc, gyr);
  //   // return filt->getEuler();
  //   return NULL;
  // }

  float getZ() {
    return acc[2];
  }

  float acc[3];
  float gyr[3];
protected:
  // OrientationFilter *filt;

  // These should be set by readSensors, and be in units of m/s^2 and rad/s
};

/** @} */ // end of addtogroup

#endif
