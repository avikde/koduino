#ifndef ComplementaryFilter_h
#define ComplementaryFilter_h

#include "OrientationFilter.h"
#include <math.h>

// cldoc:begin-category(IMU)


class ComplementaryFilter : public OrientationFilter {
public:
  ComplementaryFilter(float dt, float smooth, float acclb, float accub, bool flipz) : dt(dt), smooth(smooth), acclb(acclb), accub(accub), flipz(flipz) {}
  ComplementaryFilter() : dt(0.001), smooth(0.5), acclb(0.5), accub(2), flipz(false) {}

  // Main function
  void update(const float *acc, const float *gyr) {
    static float pitchAcc, rollAcc;
    // Integrate the gyroscope data -> int(angularSpeed) = angle
    // Angle around the X-axis
    euler.angles[0] += gyr[0] * dt;
    // Angle around the Y-axis
    euler.angles[1] += gyr[1] * dt;
    // Angle around the Z-axis
    euler.angles[2] += gyr[2] * dt;

    // Compensate for drift with accelerometer data if within range of good accels
    float accmag = fabs(acc[0]) + fabs(acc[1]) + fabs(acc[2]);
    if (accmag > acclb && accmag < accub) {
      if (flipz) {
        rollAcc = atan2f(acc[1], -acc[2]);
        pitchAcc = -atan2f(acc[0], -acc[2]);
      } else {
        rollAcc = atan2f(acc[1], acc[2]);
        pitchAcc = -atan2f(acc[0], acc[2]);  
      }
      euler.angles[0] = euler.angles[0] * smooth + rollAcc * (1.0-smooth);
      euler.angles[1] = euler.angles[1] * smooth + pitchAcc * (1.0-smooth);
    }
  }

  const EulerState *getEuler() {
    // Calculations already use an Euler representation
    return &euler;
  }
  // // testing?
  // float getZ(const IMU *imu) {
  //   return imu->acc[2];
  // }

  // Params
  float dt, smooth;
  float acclb, accub;
  bool flipz;

protected:
};

// cldoc:end-category()

#endif
