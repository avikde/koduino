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
#ifndef _timer_h_
#define _timer_h_

#include "chip.h"
#include "types.h"
#include "exti.h"
#include "gpio.h"

#ifdef __cplusplus
extern "C"{
#endif // __cplusplus

// FUTURE IDEAS?
// 
// pinModeTimer should take another argument: ENCODER_AB, PWM_IN or PWM_OUT
// if pwmIn is called and it is in output mode it should be re-inited TIM_ICInit etc. <- function timerInitPulseInPin
// if analogWrite is called and it is in input mode it should be re-inited TIM_OC1Init etc. <- timerInitPWMPin


/** @addtogroup PWM PWM output and input
 *  @{
 */

/**
 * @brief Set the resolution for the analogWrite() function
 * @details Note that this has no effect on the actual PWM resolution, which is determined by the period of the PWM (see analogWriteFrequency()). This also has no effect on analogWriteFloat()
 * 
 * @param nbits Number of bits used in analogWrite() (default is 8, as in Arduino)
 */
void analogWriteResolution(uint8_t nbits);

/**
 * @brief Assign a PWM frequency to the specified pin (default = 1000 Hz)
 * @details Please see the [pin remapping](@ref Pins) documentation for more details about 
 * connected timers to pins, including defaults. This function will change the PWM frequency for
 * all pins connected to the same timer.
 * 
 * *Gory details:*
 * * There is a 36MHz base clock.
 * * Resolution: When PWM freq = 20KHz, resolution = 36000/20 = 1800 ~= 11 bits, and better for lower frequencies
 * * Minimum freuency is 36000000/65535 = 549Hz
 * 
 * @param pin Pin to change frequency on
 * @param freqHz New frequency in Hz (default is 1000)
 */
void analogWriteFrequency(uint8_t pin, int freqHz);

/**
 * @brief Change the PWM duty cycle on a pin
 * @details Unlike Arduino, pinMode() must have been called on the pin beforehand with `PWM`.
 * Also, the duty cycle is specified in floating point, and the resolution is only limited by 
 * the timer (see analogWriteFrequency()).
 * 
 * @param pin 
 * @param duty The new duty cycle in normalized floating point from 0.0 (0%) to 1.0 (100%).
 */
void analogWriteFloat(uint8_t pin, float duty);

/**
 * @brief Change the PWM duty cycle on a pin
 * @details This should have the same effect as the Arduino function, but it is not efficient on systems without a floating point (as implemented), since it calls analogWriteFloat() under the hood.
 * 
 * @param pin 
 * @param duty The new duty cycle in [0,2^n] where n is the bit resolution set by analogWriteResolution() (default 8)
 */
void analogWrite(uint8_t pin, uint32_t duty);

/**
 * @brief Read a PWM signal (non-blocking)
 * @details pinMode() with `PWM_IN` must have been called on the pin before-hand.
 * Unlike Arduino, this function does not block and wait. It uses high-speed timer interrupts 
 * to catch rising and falling edges, and keeps track of the time when these happen to 
 * estimate the PWM duty cycle. 
 * 
 * *Advanced:*
 * 
 * * Period: The period of the PWM is also available in low-level form. The command 
 * ~~~
 * TIMER_MAP[ PIN_MAP[ *pin* ].timer ].channelData[ PIN_MAP[ *pin* ].channel-1].period
 * ~~~
 * will return the PWM period in units such that 36000 -> 1ms, 3600 -> 0.1ms etc.
 * 
 * @param pin Pin to read
 * @return A float from 0.0 to 1.0 corresponding to PWM duty cycle
 */
float pwmIn(uint8_t pin);

// To try and reduce position reading bugs, fix pwmIn period
// 36000 -> 1ms (1KHz PWM)
// Set = 0 to autodetect period
// extern int PWM_IN_FIXED_PERIOD;
/**
 * @brief Set the expected period for PWM_IN or PWM_IN_EXTI
 * @param expectedUs Expected period in microseconds (default 1000)
 */
void pwmInExpectedPeriod(int expectedUs);

// Helper functions
void pwmInRaw(uint8_t name, int *period, int *pulseWidth);


/** @} */ // end of addtogroup

void timerInit(uint8_t timer, int freqHz);

void pinTimerInit(uint8_t pin);

// Timer init with period: mostly a helper function
void timerInitHelper(uint8_t timer, uint16_t prescaler, uint32_t period);



// Helper for each channel
static inline void timerCCxISR(TIM_TypeDef *TIMx, TimerChannelData *C, int current, uint32_t currRollover) __attribute__((always_inline, unused));
static inline void timerCCxISR(TIM_TypeDef *TIMx, TimerChannelData *C, int current, uint32_t currRollover) {
  // Is this a pwmIn pin?
  if (C->bPwmIn == 1) {
    // Keep track of how many times timer rolled over since last time
    int newRollovers = currRollover - C->lastRollover;
    int delta = current + TIMx->ARR * newRollovers - C->risingEdge;

    if (digitalRead(C->pin)) {
      // This was a rising edge
      if (delta > PWM_IN_MAXPERIOD)
        C->period = delta - TIMx->ARR;
      else if (delta < PWM_IN_MINPERIOD)
        C->period = delta + TIMx->ARR;
      else 
        C->period = delta;

      C->risingEdge = current;
      C->lastRollover = currRollover;
    } else {
      // This was a falling edge
      C->pulseWidth = delta;
      
      // HACK: sometimes it is greater than the period
      if (C->pulseWidth > C->period)
        C->pulseWidth -= C->period;
      if (C->pulseWidth < 0)
        C->pulseWidth += C->period;
    }
  }
}

// Main GP timer ISR
static inline void timerISR(uint8_t timer) __attribute__((always_inline, unused));
static inline void timerISR(uint8_t timer) {
  TIM_TypeDef *TIMx = TIMER_MAP[timer].TIMx;
  TimerInfo *cfg = &TIMER_MAP[timer];

  // Update for rollover
  if (TIM_GetITStatus(TIMx, TIM_IT_Update) != RESET) {
    TIM_ClearITPendingBit(TIMx, TIM_IT_Update);
    cfg->numRollovers++;
  }

  // CCx for pwmIn
  if (TIM_GetITStatus(TIMx, TIM_IT_CC1) != RESET) {
    TIM_ClearITPendingBit(TIMx, TIM_IT_CC1);
    timerCCxISR(TIMx, &cfg->channelData[0], TIM_GetCapture1(TIMx), cfg->numRollovers);
  }
  if (TIM_GetITStatus(TIMx, TIM_IT_CC2) != RESET) {
    TIM_ClearITPendingBit(TIMx, TIM_IT_CC2);
    timerCCxISR(TIMx, &cfg->channelData[1], TIM_GetCapture2(TIMx), cfg->numRollovers);
  }
  if (TIM_GetITStatus(TIMx, TIM_IT_CC3) != RESET) {
    TIM_ClearITPendingBit(TIMx, TIM_IT_CC3);
    timerCCxISR(TIMx, &cfg->channelData[2], TIM_GetCapture3(TIMx), cfg->numRollovers);
  }
  if (TIM_GetITStatus(TIMx, TIM_IT_CC4) != RESET) {
    TIM_ClearITPendingBit(TIMx, TIM_IT_CC4);
    timerCCxISR(TIMx, &cfg->channelData[3], TIM_GetCapture4(TIMx), cfg->numRollovers);
  }
}


// Special timer features
void complementaryPWM(uint8_t timer, int freqHz, uint16_t deadtime);


#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus

#endif

