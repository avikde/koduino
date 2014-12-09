
#include "system_clock.h"

#include <stm32f37x_misc.h>
#include <stm32f37x_tim.h>
#include <stm32f37x_rcc.h>

#include "nvic.h"
#include "timer.h"
#include "gpio.h"
#include "timebase.h"

// Global
volatile uint32_t _millis = 0;

// // How fast is loop() running? This is useful for speed-dependent calculations like D-gains. This is used in main.c
// RateMeasurerType loopRate;
// float loopRateHz = 0;

void systemClockInit()
{
  attachSysClkInterrupt(systemClockISR);
}

void systemClockISR()
{
  _millis++;
}

uint32_t millis()
{
  return _millis;
}

uint32_t micros()
{
  // return _millis * 1000 + TIM_GetCounter(TIMER_MAP[sysClkTimer].TIMx);
  return _millis * 1000 + TIM_GetCounter(TIM18);
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

void delayNB(uint32_t nTime, IterFunc iterate)
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



