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
#include <stdio.h>
#include "chip.h"

#include "timebase.h"
#include "gpio.h"
#include "variant.h"
#include "nvic.h"

// Timers are on APB1 or APB2, which are not necessarily the system clock
#if defined(STM32F40_41xxx)
// In the clock config tool, APB1,APB2 clocks set to 84 MHz
#define TIMER_PERIPH_CLOCK 84000000
#else
#define TIMER_PERIPH_CLOCK SystemCoreClock
#endif

// Helper function
void setupTimerUpdateInterrupt(uint8_t i, ISRType ISR, uint16_t prescaler, uint32_t period) {
  uint8_t timer = TIMEBASE_MAP[i].timer;
  timerInitHelper(timer, prescaler, period);
  TIM_ITConfig(TIMER_MAP[timer].TIMx, TIM_IT_Update, ENABLE);
  TIM_Cmd(TIMER_MAP[timer].TIMx, ENABLE);
  // Start interrupts with low priority
  nvicEnable(TIMER_MAP[timer].IRQn, 0xf);
  
  TIMEBASE_MAP[i].isr = ISR;
}

// OLD...now using SysTick
// void attachSysClkInterrupt(ISRType ISR) {
//   // 1MHz base clock, millisecond interrupts, microsecond counts
//   setupTimerUpdateInterrupt(SYSCLK_TIMEBASE, ISR, (TIMER_PERIPH_CLOCK / 1000000)-1, 1000-1);
// }

// void attachHighResClkUpdate(ISRType ISR) {
//   // Highest resolution - 16bit
//   // 4 KHz timer - make sure signals are not faster than this!!!
//   // Tradeoff is: low prescaler => more precise, but will overflow the 32-bit int holding the tick value faster.
//   setupTimerUpdateInterrupt(3, ISR, (TIMER_PERIPH_CLOCK / 4000000)-1, 65535);
// }

void attachTimerInterrupt(uint8_t i, ISRType ISR, int freqHz) {
  // Ideally, (without the -1) period = prescaler = sqrt(72MHz/freq), but sqrt is expensive
  // Approximate by having 2 cases
  if (freqHz >= 2000) {
    setupTimerUpdateInterrupt(i, ISR, 0, (TIMER_PERIPH_CLOCK / freqHz)-1);
  } else {
    // 4Khz base clock after prescaling
    // period = 2 => 2KHz, period = 0xffff => 0.061Hz
    uint32_t period = 4000 / freqHz - 1;
    if (period < 1 || period > 0xffff)
      return;

    setupTimerUpdateInterrupt(i, ISR, (TIMER_PERIPH_CLOCK / 4000)-1, period);
  }
}

void detachTimerInterrupt(uint8_t i)
{
  TIMEBASE_MAP[i].isr = 0;
  TIM_ITConfig(TIMER_MAP[TIMEBASE_MAP[i].timer].TIMx, TIM_IT_Update, DISABLE);
}

void noTimerInterrupts()
{
  for (int i = 0; i < 2; ++i)
    NVIC_DisableIRQ(TIMER_MAP[TIMEBASE_MAP[i].timer].IRQn);
}

void timerInterrupts()
{
  for (int i = 0; i < 2; ++i)
    NVIC_EnableIRQ(TIMER_MAP[TIMEBASE_MAP[i].timer].IRQn);
}

