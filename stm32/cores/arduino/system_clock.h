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
#ifndef _system_clock_
#define _system_clock_

// Uses interrupts and timer 16 for millis() and micros() time keeping
#include <stdint.h>
#include "chip.h"
#include "types.h"

#ifdef __cplusplus
extern "C"{
#endif // __cplusplus


/// Initialize clock counter and timer hardware.
void systemClockInit();

// Private
void systemClockISR();


/** @addtogroup Time Keeping time
 *  @{
 */
extern volatile uint32_t _millis;
extern int microsDivider;
/**
 * @brief 
 * @details Rolls over at max of uint32_t or ~50 days
 * @return Time elapsed in milliseconds since the start of the program
 */
static inline uint32_t millis() __attribute__((always_inline, unused));
static inline uint32_t millis() {
  volatile uint32_t ret = _millis;
  return ret;
}


/**
 * @brief 
 * @details Rolls over at max of uint32_t or ~71 minutes
 * @return Time elapsed in microseconds since the start of the program
 */
static inline uint32_t micros() __attribute__((always_inline, unused));
static inline uint32_t micros()
{
  // OLD--using timer
  // return _millis * 1000 + TIM_GetCounter(TIMER_MAP[ TIMEBASE_MAP[ SYSCLK_TIMEBASE ].timer ].TIMx);
  // systick is a downcounter
  volatile uint32_t ret = _millis * 1000 + 1000 - SysTick->VAL/microsDivider;
  return ret;
}

/**
 * @brief Blocks execution for a specified number of milliseconds
 * @details Only accurate to +/- 1 ms; if you need more precision use delayMicroseconds(). 
 * Note that any code called through [timer interrupts](@ref Timebase) will still execute.
 * 
 * @param nTime Number of milliseconds to wait
 */
void delay(uint32_t nTime);

/**
 * @brief Blocks execution for a specified number of microseconds
 * @details Note that any code called through [timer interrupts](@ref Timebase) will still execute.
 * 
 * @param nTime Number of microseconds to wait
 */
void delayMicroseconds(uint32_t nTime);

/**
 * @brief Non-blocking delay
 * @details Will repeatedly execute the callback function while waiting. Note, it is 
 * preferred to use [timer interrupts](@ref Timebase) for this kind of functionality. 
 * **FIXME:** test if timebase screws up Wire or Serial communication
 * 
 * @param nTime Number of milliseconds to wait
 * @param iterate Function to call while waiting
 */
void delayNB(uint32_t nTime, ISRType iterate);

/** @} */ // end of addtogroup



#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus

#endif
