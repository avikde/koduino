
#include "timer.h"
#include "nvic.h"
#include "gpio.h"
#include "exti.h"
#include "system_clock.h"

// Globals ---------------------------------------------------------

// PWM period needed for a certain PWM frequency
// assuming prescaler=1 => TIMER_PERIPH_CLOCK / 2
#if defined(SERIES_STM32F37x)
#define TIMER_BASE_CLOCK 36000000
#elif defined(SERIES_STM32F30x)
#define TIMER_BASE_CLOCK 24000000
#else
#define TIMER_BASE_CLOCK 42000000
#endif
#define TIMER_PERIOD(freq_hz) ((int)TIMER_BASE_CLOCK/(freq_hz) - 1)

// Re-used to init PWM channels
TIM_OCInitTypeDef  TIM_OCInitStructure;
uint8_t _analogWriteResolution = 8;

int PWM_IN_FIXED_PERIOD = 0;

//------------------------------------------------------------------

void timerInitHelper(uint8_t timer, uint16_t prescaler, uint32_t period) {
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  TIM_TimeBaseStructure.TIM_Prescaler = prescaler;
  TIM_TimeBaseStructure.TIM_Period = period;
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
  nvicEnable(TIMER_MAP[timer].IRQn, 0);

  timerInitHelper(timer, 1, TIMER_PERIOD(freqHz));
  TIM_Cmd(TIMER_MAP[timer].TIMx, ENABLE);
}


void pinTimerInit(uint8_t pin) {
  uint8_t timer = PIN_MAP[pin].timer;

  nvicEnable(TIMER_MAP[timer].IRQn, 0);
  // Use the frequency set using analogWriteFrequency
  timerInitHelper(timer, 1, TIMER_PERIOD(TIMER_MAP[timer].freqHz));
  TIM_Cmd(TIMER_MAP[timer].TIMx, ENABLE);
  
  // FIXME: Advanced timers need to be specified in variant.cpp
  // isAdvancedTimer() ir something
  // http://www.disca.upv.es/aperles/arm_cortex_m3/curset/STM32F4xx_DSP_StdPeriph_Lib_V1.0.1/html/group___t_i_m___group4.html
#if defined(SERIES_STM32F4xx)
  if (TIMER_MAP[timer].TIMx == TIM1) {
    TIM_CtrlPWMOutputs(TIM1, ENABLE);
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
    timediff = micros() - S->risingedge;
    // HACK: need to detect if the signal goes flat. 5ms = 5000us
    if (timediff > 5000)
      return 0;

    return (S->period > 0) ? S->pulsewidth/(float)S->period : 0;
  }

  // assume this is a PWM_IN pin using timer input channels

  uint8_t timer = PIN_MAP[name].timer;
  TimerChannelData *C = &TIMER_MAP[ timer ].channelData[ PIN_MAP[name].channel-1];

  // HACK: need to detect if the signal goes flat. Each rollover is 1ms @1KHz
  if (TIMER_MAP[timer].numRollovers - C->lastRollover > 5)
    return 0;

  if (PWM_IN_FIXED_PERIOD == 0)
    return (C->period > 0 && C->period >= C->pulseWidth) ? C->pulseWidth/(float)C->period : 0;
  else
    return C->pulseWidth/(float)PWM_IN_FIXED_PERIOD;
}

void pwmInRaw(uint8_t name, int *period, int *pulseWidth) {
  uint8_t timer = PIN_MAP[name].timer;
  TimerChannelData *C = &TIMER_MAP[ timer ].channelData[ PIN_MAP[name].channel-1];

  *period = (PWM_IN_FIXED_PERIOD > 0) ? PWM_IN_FIXED_PERIOD : C->period;
  *pulseWidth = C->pulseWidth;
}

// Main ISR =============================================

// Helper for each channel
void timerCCxISR(TIM_TypeDef *TIMx, TimerChannelData *C, int current, uint32_t currRollover) {
  // Is this a pwmIn pin?
  if (C->bPwmIn == 1) {
    // Keep track of how many times timer rolled over since last time
    int newRollovers = currRollover - C->lastRollover;

    if (digitalRead(C->pin)) {
      if (PWM_IN_FIXED_PERIOD == 0) {
        // This was a rising edge
        int newPeriod = current + TIMx->ARR * newRollovers - C->risingEdge;
        // HACK: 
        // symptom: sometimes the period is ~double what expected
        // compare to previous period
        // if (!(abs(newPeriod - 2*C->period) < 1000))
          C->period = newPeriod;
      } else {
        C->period = PWM_IN_FIXED_PERIOD;
      }
      C->risingEdge = current;
      C->lastRollover = currRollover;
    } else {
      // This was a falling edge
      C->pulseWidth = current + TIMx->ARR * newRollovers - C->risingEdge;
      
      // HACK: sometimes it is greater than the period
      if (C->pulseWidth > C->period)
        C->pulseWidth -= C->period;
      if (C->pulseWidth < 0)
        C->pulseWidth += C->period;
      // if (PWM_IN_FIXED_PERIOD == 0) {
      // } else 
      //   C->pulseWidth = (C->pulseWidth % PWM_IN_FIXED_PERIOD);
    }
  }
}

void timerISR(uint8_t timer) {
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
  TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;

  TIM_OC1Init(TIMER_MAP[timer].TIMx, &TIM_OCInitStructure);
  TIM_OC2Init(TIMER_MAP[timer].TIMx, &TIM_OCInitStructure);
  TIM_OC3Init(TIMER_MAP[timer].TIMx, &TIM_OCInitStructure);

  /* Automatic Output enable, Break, dead time and lock configuration*/
  TIM_BDTRInitStructure.TIM_OSSRState = TIM_OSSRState_Enable;
  TIM_BDTRInitStructure.TIM_OSSIState = TIM_OSSIState_Enable;
  TIM_BDTRInitStructure.TIM_LOCKLevel = TIM_LOCKLevel_1;
  TIM_BDTRInitStructure.TIM_DeadTime = deadtime;
  TIM_BDTRInitStructure.TIM_Break = TIM_Break_Enable;
  TIM_BDTRInitStructure.TIM_BreakPolarity = TIM_BreakPolarity_High;
  TIM_BDTRInitStructure.TIM_AutomaticOutput = TIM_AutomaticOutput_Enable;

  TIM_BDTRConfig(TIMER_MAP[timer].TIMx, &TIM_BDTRInitStructure);

  /* counter enable */
  TIM_Cmd(TIMER_MAP[timer].TIMx, ENABLE);

  /* Main Output Enable */
  TIM_CtrlPWMOutputs(TIMER_MAP[timer].TIMx, ENABLE);
  
#endif
}

