
#include "EKF.h"
#include <Arduino.h>

//  Eigen does not define these operators, which we use for integration
static inline quat operator + (const quat& a, const quat& b) {
  return quat(a.w()+b.w(), a.x()+b.x(), a.y()+b.y(), a.z()+b.z());
}

static inline quat operator * (const quat& q, float s) {
  return quat(q.w() * s, q.x() * s, q.y() * s, q.z() * s);
}

/**
 *  @brief Integrate a rotation quaterion using Euler integration
 *  @param q Quaternion to integrate
 *  @param w Angular velocity (body frame), stored in 3 complex terms
 *  @param dt Time interval in seconds
 *  @param normalize If True, quaternion is normalized after integration
 */
static inline void integrateEuler(quat &q, quat &w, float dt, bool normalize = true) {
  q = q + (q * w * 0.5) * dt;
  if (normalize) {
    q.normalize();
  }
}

//  skew symmetric matrix
static inline mat3 crossSkew(const vec3 &w) {
  mat3 W;

  W << 0, -w(2), w(1),
    w(2), 0, -w(0),
    -w(1), w(0), 0;

  return W;
}

EKF::EKF() : dt(0.001) {
  gravity << 0, 0, kOneG;

  P.setZero();
  b.setZero();
  w.setZero();
  dx.setZero();

  // state noise
  wCov = 1 * mat3::Identity();
  // measurement noise
  aCov = 1 * mat3::Identity();
  // estBias_ = false;
  // ignoreZ_ = false;
  // useMag_ = false;
}

void EKF::init(const float *acc) {
  //  determine attitude angles
  float ay = constrain(acc[1], -kOneG, kOneG);

  float roll = asinf(-ay / kOneG);  //  roll
  float pitch = atan2f(acc[0], acc[2]);    //  pitch

  q = Eigen::AngleAxis<float>(pitch, vec3(0,1,0)) * 
       Eigen::AngleAxis<float>(roll, vec3(1,0,0));

  //  start w/ a large uncertainty
  P.setIdentity();
  P *= 10;
}


void EKF::predict(const float *gyr) {
  // static const mat3 I = mat3::Identity(); //  identity R3

  // // If steady do a moving average bias estimate
  // float wb2 = gyr[0] * gyr[0] + gyr[1] * gyr[1] + gyr[2] * gyr[2];
  // if (wb2 < biasThresh_ * biasThresh_) {
  //   steadyCount_++; //  not rotating, update moving average

  //   if (estBias_ && steadyCount_ > 20) {
  //     b = (b * (steadyCount_ - 1) + wb) / steadyCount_;
  //   }
  // } else {
  //   steadyCount_ = 0;
  // }
  // w = (wb - b); //  true gyro reading

  w << gyr[0], gyr[1], gyr[2];

  //  error-state jacobian
  mat3 F = I - crossSkew(w * dt);

  //  integrate state and covariance
  quat wQuat(0, w[0], w[1], w[2]);
  integrateEuler(q, wQuat, dt, true);

  //  noise jacobian
  // const Matrix <float,3,3> G = -dt;
  P = F * P * F.transpose() + (dt*dt) * wCov;
}

// Main function
void EKF::update(const float *acc, const float *gyr) {
  // prediction step
  predict(gyr);

  mat3 A;

  //  rotation matrix: world -> body
  mat3 bRw = q.conjugate().matrix();

  //  predicted gravity vector
  vec3 aPred = bRw * gravity;

  //  calculate jacobian
  mat3 H = crossSkew(aPred);
  vec3 y = toVec3((float *)acc) - aPred;

  //  solve for the kalman gain
  mat3 S = H * P * H.transpose() + aCov;
  mat3 Sinv;

  // const float det = S.determinant();
  // if (std::abs(det) < static_cast<float>(1e-5)) {
  //   isStable_ = false;
  //   return;
  // } else {
  //   isStable_ = true;
  // }
  Sinv = S.inverse();

  const mat3 K = P * H.transpose() * Sinv;

  A = K * H;
  dx = K * y;

  //  perform state update
  P = (I - A) * P;

  q = q * quat(1, dx[0]/2, dx[1]/2, dx[2]/2);
  q.normalize();
}

const EulerState *EKF::getEuler() {
  mat3 R = q.matrix();
  float sth = constrain(-R(2, 0), -1, 1);
  
  euler.angles[1] = asinf(sth);
  float cth;
  arm_sqrt_f32(1 - sth*sth, &cth);
  
  if (cth < 1.0e-6) {
    euler.angles[0] = atan2f(R(0, 1), R(1, 1));
    euler.angles[2] = 0;
  } else {
    euler.angles[0] = atan2f(R(2, 1), R(2, 2));
    euler.angles[2] = atan2f(R(1, 0), R(0, 0));
  }

  // ang rates
  euler.angRates[0] = w[0];
  euler.angRates[1] = w[1];
  euler.angRates[2] = w[2];
  
  return &euler;
}
