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

class EKF : public OrientationFilter {
public:
  const float kOneG = 9.8;
  const mat3 I = mat3::Identity();
  vec3 gravity;
  
  EKF();
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
