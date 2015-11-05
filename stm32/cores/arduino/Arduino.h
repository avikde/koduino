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
  
#ifndef Arduino_h
#define Arduino_h

#ifdef __cplusplus
extern "C"{
#endif // __cplusplus

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define clockCyclesPerMicrosecond() ( SystemCoreClock / 1000000L )
#define clockCyclesToMicroseconds(a) ( ((a) * 1000L) / (SystemCoreClock / 1000L) )
#define microsecondsToClockCycles(a) ( (a) * (SystemCoreClock / 1000000L) )

void yield(void);

// sketch
extern void setup();
extern void loop();


#include "wiring_constants.h"
#include "gpio.h"
#include "timer.h"
#include "system_clock.h"
#include "timebase.h"
#include "adc.h"
#include "pins.h"
#include "exti.h"

#include <arm_math.h>

#ifdef __cplusplus
} // extern "C"


#include "variant.h"

// #include "WCharacter.h"
#include "WString.h"
// #include "Tone.h"
#include "WMath.h"
#include "USARTClass.h"
// Streaming library http://arduiniana.org/libraries/streaming/
#include "Streaming.h"

#endif // __cplusplus

#endif // Arduino_h
