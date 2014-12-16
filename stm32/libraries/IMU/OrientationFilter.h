#ifndef OrientationFilter_h
#define OrientationFilter_h

/** @addtogroup IMU
 *  @{
 */

enum OrientationFilterType {
  IMU_FILTER_BUILTIN, IMU_FILTER_COMPLEMENTARY, IMU_FILTER_UKF
};

typedef struct {
  float angles[3];
  float angRates[3];
} EulerState;


// cldoc:begin-category(IMU)

// Base class for filters
class OrientationFilter {
public:
  // Update into whatever internal state
  virtual void update(const float *acc, const float *gyr) = 0;

  // Must be able to return euler angles
  virtual const EulerState *getEuler() = 0;
  
protected:
  EulerState euler;
};

/** @} */ // end of addtogroup

#endif
