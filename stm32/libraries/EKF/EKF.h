/**
 * @authors Gareth Cross, Avik De <avikde@gmail.com>

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
#ifndef EKF_h
#define EKF_h

#include "OrientationFilter.h"
#include <Eigen.h>

/** @addtogroup IMU
 *  @{
 */

#ifdef __cplusplus

typedef Eigen::Matrix<float, 3, 1> vec3;
typedef Eigen::Matrix<float, 3, 3> mat3;
typedef Eigen::Quaternion<float> quat;

static inline mat3 toMat3(float *arr) {
  return Eigen::Map<mat3, Eigen::RowMajor>(arr);
}

static inline vec3 toVec3(float *arr) {
  return Eigen::Map<vec3, Eigen::RowMajor>(arr);
}

// mat3 toEigenM(const float *arr) {
//   mat3 out;
//   out << arr[0], arr[1], arr[2], arr[3], arr[4], arr[5], arr[6], arr[7], arr[8];
//   return out;
// }

/**
 * @brief EKF implementation
 */
class EKF : public OrientationFilter {
public:
  const float kOneG = 9.8;
  const mat3 I = mat3::Identity();
  vec3 gravity;
  
  EKF(float stateNoise, float measNoise, float dt);
  EKF() : EKF(0.1, 10, 0.001) {}
  void init(const float *acc);

  // Main function
  void update(const float *acc, const float *gyr);

  const EulerState *getEuler();
  // // testing?
  // float getZ(const IMU *imu) {
  //   return imu->acc[2];
  // }

  // // Params
  // float dt, smooth;
  // float acclb, accub;
  // bool flipz;

  mat3 A, B, C;
protected:
  void predict(const float *gyr);

  EulerState euler;

  quat q; /// Orientation
  mat3 P; /// System covariance

  vec3 w;
  vec3 b;
  vec3 dx;

  float dt;
  mat3 wCov, aCov;
};

#endif // __cplusplus

/** @} */ // end of addtogroup

#endif
