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
