/**
 * @authors Particle Industries, Avik De <avikde@gmail.com>

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
  
#ifndef _exti_h_
#define _exti_h_

#include "types.h"
#include "system_clock.h"
#include "gpio.h"

#ifdef __cplusplus
extern "C"{
#endif // __cplusplus

extern int PWM_IN_EXTI_MAXPERIOD;
extern int PWM_IN_EXTI_MINPERIOD;
extern int PWM_IN_MAXPERIOD;
extern int PWM_IN_MINPERIOD;

extern const uint32_t extiLines[];
// This function is called by any of the interrupt handlers. It essentially fetches the user function pointer from the array and calls it.
static inline void wirishExternalInterruptHandler(uint8_t i) __attribute__((always_inline, unused));
static inline void wirishExternalInterruptHandler(uint8_t i) {
  EXTI_ClearITPendingBit(extiLines[i]);
  __disable_irq();
  EXTIChannel *S = &EXTI_MAP[i];
  // PWM_IN_EXTI pin?
  if (S->bPwmIn == 1) {
    
    // NEW
    volatile int currentMs = millis();
    volatile int currentSubMs = SysTick->LOAD - SysTick->VAL;
    volatile int delta = (currentMs - S->risingEdgeMs) * SysTick->LOAD + currentSubMs - S->risingEdgeSubMs;

    if (digitalRead(S->pinName)) {
      // This was a rising edge
      S->risingEdgeMs = currentMs;
      S->risingEdgeSubMs = currentSubMs;
      S->period = delta;
      if (delta > PWM_IN_EXTI_MAXPERIOD)
        S->period = delta - SysTick->LOAD;
      else if (delta < PWM_IN_EXTI_MINPERIOD)
        S->period = delta + SysTick->LOAD;
    } else {
      // This was a falling edge
      S->pulsewidth = delta;
      if (S->pulsewidth < 0)
        S->pulsewidth += S->period;
      if (S->pulsewidth > S->period)
        S->pulsewidth -= S->period;
    }
  } else {
    // ELSE fetch the user function pointer from the array
    ISRType handler = S->handler;
    //Check to see if the user handle is NULL
    if (handler != 0)
      handler();
  }
  __enable_irq();
}

/** @addtogroup EXTI External interrupts
 *  @{
 */

void attachInterruptWithPriority(uint8_t pinName, ISRType ISR, InterruptTrigger mode, uint8_t priority);

/**
 * @brief Enable an interrupt with default priority
 * @details **Note:** There is only ONE EXTI per *pinSource*, i.e. 
 * PA0, PB0 cannot both have interrupts, but PA0 and PB1 can. 
 * Obviously, there can be a total of 16.
 * 
 * @param pinName Name of pin interrupt is connected to
 * @param ISR Callback function to call when interrupt is triggered
 * @param mode One of InterruptTrigger
 */
void attachInterrupt(uint8_t pinName, ISRType ISR, InterruptTrigger mode);

/**
 * @brief Disable an interrupt
 * @details 
 * 
 * @param pinName Pin name where attachInterrupt() was called
 */
void detachInterrupt(uint8_t pinName);

/**
 * @brief Disable external interrupts
 * @details **Note:** This is unlike the Arduino function which disables ALL interrupts. 
 * That would stop the system clock here as well.
 * Call `NVIC_DisableIRQ()` if you want the arduino behavior
 */
void noInterrupts();

/**
 * @brief Enable external interrupts
 * @details Enabled by default
 */
void interrupts();


/** @} */ // end of addtogroup



// // PWM Input using EXTI
// float pwmIn(uint8_t pinName);

// // Private
// void pinModePwmIn(uint8_t pinName);
// void pwmInClkInit();
// uint32_t pwmInTicks();

#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus

#endif
