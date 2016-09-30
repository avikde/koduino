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

typedef struct {
  float angles[3];
  float angRates[3];
} EulerState;


/**
 * @brief Base class for attitude filters
 */
class OrientationFilter {
public:
  /**
   * @brief Update filter from measurements (usually from IMU.readSensors())
   * @param acc Measured acc
   * @param gyr Measured gyr
   */
  virtual void update(const float *acc, const float *gyr) = 0;

  /**
   * @brief Get filtered Euler angles
   * @return `e->angles[]` is a `float[3]` array of angles and, `e->angRates[]` of angular rates
   */
  virtual const EulerState *getEuler() = 0;
  
protected:
  EulerState euler;
};

/** @} */ // end of addtogroup

#endif
