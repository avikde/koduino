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
#include "timer.h"
#include "nvic.h"
#include "gpio.h"
#include "exti.h"
#include "system_clock.h"

// Globals ---------------------------------------------------------


uint8_t TIMER_IC_PRIORITY = 0;
// Re-used to init PWM channels
TIM_OCInitTypeDef  TIM_OCInitStructure;
uint8_t _analogWriteResolution = 8;


// Helper function that calculates the PWM period needed for a certain PWM frequency
// assuming prescaler=1
uint32_t timerCalculatePeriod(int freqHz, TIM_TypeDef *TIMx) {
#if defined(STM32F446xx)
  // New: calculate properly
  RCC_ClocksTypeDef RCC_Clocks;
  RCC_GetClocksFreq(&RCC_Clocks);
  uint32_t PCLK = RCC_Clocks.PCLK1_Frequency;// Frequency in Hz
  // For timers on APB2 use PCLK2
  if (TIMx == TIM1 || TIMx == TIM8 || TIMx == TIM9 || TIMx == TIM10 || TIMx == TIM11)
    PCLK = RCC_Clocks.PCLK2_Frequency;
  return PCLK/(freqHz) - 1;
#else
#if defined(SERIES_STM32F37x)
  uint32_t TIMER_BASE_CLOCK = 36000000;
#elif defined(SERIES_STM32F30x)
  uint32_t TIMER_BASE_CLOCK = SystemCoreClock/2;
#else
  // F405
  uint32_t TIMER_BASE_CLOCK = 42000000;
#endif
  // Old: use macro
  return (TIMER_BASE_CLOCK/freqHz - 1);
#endif
}


void pwmInExpectedPeriod(int expectedUs) {
  PWM_IN_EXTI_MAXPERIOD = SysTick->LOAD/1000*((int)(1.8 * expectedUs));
  PWM_IN_EXTI_MINPERIOD = SysTick->LOAD/1000*((int)(0.2 * expectedUs));
  // different units for PWM_IN
  // NOTE just using TIM2 here assuming it exists and the code will compile on all chips
  PWM_IN_MAXPERIOD = (int)(1.8 * timerCalculatePeriod(1000000/expectedUs, TIM2));
  PWM_IN_MINPERIOD = (int)(0.2 * timerCalculatePeriod(1000000/expectedUs, TIM2));
}


//------------------------------------------------------------------

void timerInitHelper(uint8_t timer, uint16_t prescaler, uint32_t period) {
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  // Update event period = TIM_CLK/((PSC + 1)*(ARR + 1)*(RCR + 1))
  // For 16 bit timers, period (ARR) must be <= 0xffff
  // Usually use PSC=1, but if TIM_CLK is high, for low frequencies ARR would overflow. 
  // Just one quick hack for those situations: go from PSC=1 -> PSC=3. Then period should be halved
  if (period >= 0xffff && prescaler == 1) {
    TIM_TimeBaseStructure.TIM_Prescaler = 3;
    TIM_TimeBaseStructure.TIM_Period = period/2;
  } else {
    TIM_TimeBaseStructure.TIM_Prescaler = prescaler;
    TIM_TimeBaseStructure.TIM_Period = period;
  }
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  
  TIM_TimeBaseInit(TIMER_MAP[timer].TIMx, &TIM_TimeBaseStructure);
}


void analogWriteFrequency(uint8_t pin, int freqHz) {
  TIMER_MAP[ PIN_MAP[pin].timer ].freqHz = freqHz;
}

void analogWriteResolution(uint8_t nbits) {
  _analogWriteResolution = nbits;
}

void timerInit(uint8_t timer, int freqHz) {
  // Enable interrupts for the timer (but not any of the timer updates yet)
  nvicEnable(TIMER_MAP[timer].IRQn, TIMER_IC_PRIORITY);
  timerInitHelper(timer, 1, timerCalculatePeriod(freqHz, TIMER_MAP[timer].TIMx));
  TIM_Cmd(TIMER_MAP[timer].TIMx, ENABLE);
}


void pinTimerInit(uint8_t pin) {
  uint8_t timer = PIN_MAP[pin].timer;

  nvicEnable(TIMER_MAP[timer].IRQn, TIMER_IC_PRIORITY);
  // Use the frequency set using analogWriteFrequency
  timerInitHelper(timer, 1, timerCalculatePeriod(TIMER_MAP[timer].freqHz, TIMER_MAP[timer].TIMx));
  TIM_Cmd(TIMER_MAP[timer].TIMx, ENABLE);
  
  // FIXME: Advanced timers need to be specified in variant.cpp
  // isAdvancedTimer() ir something
  // http://www.disca.upv.es/aperles/arm_cortex_m3/curset/STM32F4xx_DSP_StdPeriph_Lib_V1.0.1/html/group___t_i_m___group4.html
#if defined(SERIES_STM32F4xx)
  if (IS_TIM_LIST4_PERIPH(TIMER_MAP[timer].TIMx)) {
    TIM_CtrlPWMOutputs(TIMER_MAP[timer].TIMx, ENABLE);
  }
#endif
#if defined(SERIES_STM32F30x)
  if (IS_TIM_LIST6_PERIPH(TIMER_MAP[timer].TIMx)) {
    TIM_CtrlPWMOutputs(TIMER_MAP[timer].TIMx, ENABLE);
  }
#endif
}


void analogWriteFloat(uint8_t name, float duty) {
  TIM_TypeDef *TIMx = TIMER_MAP[ PIN_MAP[name].timer ].TIMx;
  uint8_t channel = PIN_MAP[name].channel;
  switch (channel) {
  case 1:
    TIM_SetCompare1(TIMx, (uint32_t)(duty*TIMx->ARR));
    break;
  case 2:
    TIM_SetCompare2(TIMx, (uint32_t)(duty*TIMx->ARR));
    break;
  case 3:
    TIM_SetCompare3(TIMx, (uint32_t)(duty*TIMx->ARR));
    break;
  case 4:
    TIM_SetCompare4(TIMx, (uint32_t)(duty*TIMx->ARR));
    break;
  }
}

void analogWrite(uint8_t pin, uint32_t duty) {
  analogWriteFloat(pin, duty/((float)((1<<_analogWriteResolution) - 1)));
}

float pwmIn(uint8_t name) {
  EXTIChannel *S = &EXTI_MAP[ PIN_MAP[name].pin ];
  // Pin configured for PWM_IN_EXTI?
  if (S->pinName == name) {
    static int timediff;
    timediff = millis() - S->risingEdgeMs;
    // HACK: need to detect if the signal goes flat. 5ms = 5000us
    if (timediff > 5)
      return 0;

    if (S->pulsewidth < 0)
      S->pulsewidth += S->period;
    if (S->pulsewidth > S->period)
      S->pulsewidth -= S->period;
    return (S->period > 0) ? S->pulsewidth/(float)S->period : 0;
  }

  // assume this is a PWM_IN pin using timer input channels

  uint8_t timer = PIN_MAP[name].timer;
  TimerChannelData *C = &TIMER_MAP[ timer ].channelData[ PIN_MAP[name].channel-1];

  // HACK: need to detect if the signal goes flat. Each rollover is 1ms @1KHz
  if (TIMER_MAP[timer].numRollovers - C->lastRollover > 5)
    return 0;

  // if (PWM_IN_FIXED_PERIOD == 0)
    return (C->period > 0 && C->period >= C->pulseWidth) ? C->pulseWidth/(float)C->period : 0;
  // else
  //   return C->pulseWidth/(float)PWM_IN_FIXED_PERIOD;
}

void pwmInRaw(uint8_t name, int *period, int *pulseWidth) {
  EXTIChannel *S = &EXTI_MAP[ PIN_MAP[name].pin ];
  // Pin configured for PWM_IN_EXTI?
  if (S->pinName == name) {
    if (S->pulsewidth < 0)
      S->pulsewidth += S->period;
    if (S->pulsewidth > S->period)
      S->pulsewidth -= S->period;

    *period = S->period;
    *pulseWidth = S->pulsewidth;
  } else {
    uint8_t timer = PIN_MAP[name].timer;
    TimerChannelData *C = &TIMER_MAP[ timer ].channelData[ PIN_MAP[name].channel-1];

    // *period = (PWM_IN_FIXED_PERIOD > 0) ? PWM_IN_FIXED_PERIOD : C->period;
    *period = C->period;
    *pulseWidth = C->pulseWidth;
  }
}

void attachGPTimerUpdateInterrupt(uint8_t timer, ISRType isr) {
  TimerInfo *cfg = &TIMER_MAP[timer];
  cfg->isr = isr;
  TIM_ITConfig(cfg->TIMx, TIM_IT_Update, ENABLE);
}

// Main ISR =============================================


// ==================


void complementaryPWM(uint8_t timer, int freqHz, uint16_t deadtime) {
#if defined(SERIES_STM32F30x)
  // init timebase
  timerInit(timer, freqHz);

  TIM_OCInitTypeDef  TIM_OCInitStructure;
  TIM_BDTRInitTypeDef TIM_BDTRInitStructure;
  /* Channel 1, 2 and 3 Configuration in PWM mode */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;

  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
  TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
  TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
  TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Set;

  TIM_OC1Init(TIMER_MAP[timer].TIMx, &TIM_OCInitStructure);
  TIM_OC2Init(TIMER_MAP[timer].TIMx, &TIM_OCInitStructure);
  TIM_OC3Init(TIMER_MAP[timer].TIMx, &TIM_OCInitStructure);

  /* Automatic Output enable, Break, dead time and lock configuration*/
  TIM_BDTRInitStructure.TIM_OSSRState = TIM_OSSRState_Enable;
  TIM_BDTRInitStructure.TIM_OSSIState = TIM_OSSIState_Enable;
  TIM_BDTRInitStructure.TIM_LOCKLevel = TIM_LOCKLevel_OFF;
  TIM_BDTRInitStructure.TIM_DeadTime = deadtime;
  TIM_BDTRInitStructure.TIM_Break = TIM_Break_Disable;
  TIM_BDTRInitStructure.TIM_BreakPolarity = TIM_BreakPolarity_High;
  TIM_BDTRInitStructure.TIM_AutomaticOutput = TIM_AutomaticOutput_Enable;

  TIM_BDTRConfig(TIMER_MAP[timer].TIMx, &TIM_BDTRInitStructure);

  /* counter enable */
  TIM_Cmd(TIMER_MAP[timer].TIMx, ENABLE);

  /* Main Output Enable */
  TIM_CtrlPWMOutputs(TIMER_MAP[timer].TIMx, ENABLE);
  
#endif
}

