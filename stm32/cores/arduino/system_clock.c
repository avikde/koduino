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
#include "system_clock.h"

#include "chip.h"
#include "nvic.h"
#include "timer.h"
#include "gpio.h"
#include "timebase.h"
#include "exti.h"

// Global
volatile uint32_t _millis = 0;
int microsDivider = 1;

void systemClockInit()
{
  // OLD--using timer
  // attachSysClkInterrupt(systemClockISR);

  // 1ms / interrupt
  SysTick_Config(SystemCoreClock / 1000);
  // reset time
  _millis = 0;
  microsDivider = SystemCoreClock / 1000000;
  // highest priority
  NVIC_SetPriority(SysTick_IRQn, 0);

  // for PWM_IN_EXTI
  pwmInExpectedPeriod(1000);
}

// stm32 interrupt: present on all MCUs?
void SysTick_Handler() {
  _millis++;
}

//--------------------------------------------------------

void delayMicroseconds(uint32_t n)
{
  // fudge for function call overhead
  n--;
  uint32_t start = micros();
  while (micros() - start < n);
}

void delay(uint32_t n)
{
  uint32_t start = millis();
  while (millis() - start < n);
}

void delayNB(uint32_t nTime, ISRType iterate)
{
  uint32_t start = millis();
  while (millis() - start < nTime) { iterate(); }
}

// //--------------------------------


// void rateMeasurerInit(RateMeasurerType *S)
// {
//   dlpfInit(&(S->filt), 1, 0.9, DLPF_RATE);
// }

// float rateMeasurerUpdate(RateMeasurerType *S)
// {
//   // Automatically init (check if parameters are correct)
//   if (!(S->filt.frequency == 1 && S->filt.type == DLPF_RATE))
//     rateMeasurerInit(S);
//   return dlpfUpdate(&(S->filt), micros());
// }

// float rateMeasurerDt(RateMeasurerType *S)
// {
//   return S->filt.vel;
// }



