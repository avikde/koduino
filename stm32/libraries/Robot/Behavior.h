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
#ifndef Behavior_h
#define Behavior_h

/** @addtogroup Behavior
 *  @{
 */

/**
 * @brief Abstract base class for implementing behaviors
 */
class Behavior {
public:
  virtual void begin() = 0;
  virtual void update() = 0;
  virtual bool running() = 0;
  virtual void end() = 0;
  virtual void signal() {}
};

/** @} */ // end of addtogroup

#endif