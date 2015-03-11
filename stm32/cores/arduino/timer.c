
#include "timer.h"
#include "nvic.h"
#include "gpio.h"

// Globals ---------------------------------------------------------

// PWM period needed for a certain PWM frequency
// assuming prescaler=1 => TIMER_PERIPH_CLOCK / 2
#if defined(STM32F37x)
#define TIMER_BASE_CLOCK 36000000
#else
#define TIMER_BASE_CLOCK 42000000
#endif
#define TIMER_PERIOD(freq_hz) ((int)TIMER_BASE_CLOCK/(freq_hz) - 1)

// Re-used to init PWM channels
TIM_OCInitTypeDef  TIM_OCInitStructure;
uint8_t _analogWriteResolution = 8;

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
  uint8_t timer = PIN_MAP[name].timer;
  TimerChannelData *C = &TIMER_MAP[ timer ].channelData[ PIN_MAP[name].channel-1];

  // HACK: need to detect if the signal goes flat. Each rollover is 1ms @1KHz
  if (TIMER_MAP[timer].numRollovers - C->lastRollover > 5)
    return 0;

  return (C->period > 0 && C->period >= C->pulseWidth) ? C->pulseWidth/(float)C->period : 0;
  // return C->period;
}

// Main ISR =============================================

// Helper for each channel
void timerCCxISR(TIM_TypeDef *TIMx, TimerChannelData *C, int current, uint32_t currRollover) {
  // Is this a pwmIn pin?
  if (C->bPwmIn == 1) {
    // Keep track of how many times timer rolled over since last time
    int newRollovers = currRollover - C->lastRollover;

    if (digitalRead(C->pin)) {
      // This was a rising edge
      C->period = current + TIMx->ARR * newRollovers - C->risingEdge;
      // if (C->period < 0) C->period += 65535;
      C->risingEdge = current;

      C->lastRollover = currRollover;
    } else {
      // This was a falling edge
      C->pulseWidth = current + TIMx->ARR * newRollovers - C->risingEdge;
      // if (C->pulseWidth < 0) C->pulseWidth += 65535;
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


