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
#ifndef GaitRunner_h
#define GaitRunner_h

#include <Arduino.h>

/** @addtogroup Behavior
 *  @{
 */


extern const int CONTROL_RATE;

/**
 * @brief Class for clocked gaits
 */
class GaitRunner {
public:
  float duty, offset, sweep, speed;
  GaitRunner(float duty, float offset, float sweep, float speed) : duty(duty), offset(offset), sweep(sweep), speed(speed) {
    phase = 0;
  }

  virtual void update() {
    phase += speed / ((float) CONTROL_RATE);
  }

  // Buehler clock
  float legClock(float phaseOffset, float turn) {
    // "turn" > 0 speeds up this leg, <0 slows down
    // bump up the sweep and offset
    float turnSweep = sweep * (1.0 + turn);
    float turnOffset = offset;// * (1.0 + turn);

    float legphase = fmodf_0_1(phase + phaseOffset);
    // fraction into phase
    float frac;
    if (legphase < duty) {
      // Slow phase
      frac = legphase / duty;
      return turnOffset + frac * turnSweep;
    }
    else {
      // Fast phase
      frac = (legphase - duty)/(1.0 - duty);
      return turnOffset + turnSweep + frac * (TWO_PI - turnSweep);
    }
  }

  // // Tail
  // float tailClock() {
  //   // +- 0.3 in yaw ("1") direction
  //   float tailphase = fmodf_0_1(phase);
  //   return -0.25 * arm_cos_f32((tailphase) * TWO_PI);
  // }
  // // cos, cos+0.5, sin, sin+0.5

// protected:
  float phase;
};

/** @} */ // end of addtogroup

#endif
