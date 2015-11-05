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
