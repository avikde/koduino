/**
 * Part of the Wiring project - http://wiring.org.co
  Copyright (c) 2004-06 Hernando Barragan
  Modified by David A. Mellis for Arduino - http://www.arduino.cc/, Avik De <avikde@gmail.com>

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

extern "C" {
  #include "stdlib.h"
  #include "stdint.h"
}
#include "WMath.h"
#include "wiring_constants.h"
#include <arm_math.h>

extern void randomSeed( uint32_t dwSeed )
{
  if ( dwSeed != 0 )
  {
    srand( dwSeed ) ;
  }
}

extern long random( long howbig )
{
  if ( howbig == 0 )
  {
    return 0 ;
  }

  return rand() % howbig;
}

extern long random( long howsmall, long howbig )
{
  if (howsmall >= howbig)
  {
    return howsmall;
  }

  long diff = howbig - howsmall;

  return random(diff) + howsmall;
}

// Be aware of http://forum.arduino.cc/index.php?topic=46546.msg336462#msg336462
extern long map(long x, long in_min, long in_max, long out_min, long out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

extern uint16_t makeWord( uint16_t w )
{
  return w ;
}

extern uint16_t makeWord( uint8_t h, uint8_t l )
{
  return (h << 8) | l ;
}

// NEW


extern float map(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

extern float interp1(float from, float to, float frac)
{
  return (1-frac) * from + frac * to;
}
// fraction for keyframing
extern float interpFrac(uint32_t startTime, uint32_t endTime, uint32_t now)
{
  return (now - startTime)/((float)(endTime - startTime));
}

//

extern float fmodf_mpi_pi(float f)
{
  if (f>0)
    return (fmodf(f+PI, TWO_PI) - PI);
  else
    return (fmodf(f-PI, TWO_PI) + PI);
}

extern float fmodf_0_2pi(float f)
{
  float r = fmodf(f, TWO_PI);
  if (r < 0)
    r += TWO_PI;
  return r;
}

extern float fmodf_0_1(float f)
{
  float r = fmodf(f, 1.0);
  if (r < 0.0)
    r += 1.0;
  return r;
}

extern float fmodf_mp5_p5(float f)
{
  if (f>0)
    return (fmodf(f+0.5, 1) - 0.5);
  else
    return (fmodf(f-0.5, 1) + 0.5);
}

extern void circleMeanDiff(float a, float b, float *mean, float *diff)
{
  float r = fmodf_mpi_pi(a - b);
  *diff = 0.5 * r;
  *mean = fmodf_mpi_pi(b + 0.5*r);
}

//

extern void swapByte(uint8_t *a, uint8_t *b)
{
  uint8_t dummy = *b;
  *b = *a;
  *a = dummy;
}

extern void setOutPtrSafe(float *out, float val)
{
  if (out) *out = val;
}


//

float DLPF::update(float val) {
  float delta;
  switch (type)
  {
    case DLPF_ANGRATE:
      delta = fmodf_mpi_pi(val - oldVal);
      vel = interp1(delta * freq, vel, smooth);
      oldVal = val;
      return vel;
    case DLPF_RATE:
      delta = val - oldVal;
      vel = interp1(delta * freq, vel, smooth);
      oldVal = val;
      return vel;
    case DLPF_INTEGRATE:
      val = smooth * oldVal + val / freq;
      return val;
    case DLPF_SMOOTH:
    default:
      oldVal = interp1(val, oldVal, smooth);
      return oldVal;
  }
}

