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
#include "pins.h"
#include "timer.h"
#include "exti.h"

// Global variable
GPIO_InitTypeDef  GPIO_InitStruct;

void pinRemap(uint8_t pin, uint8_t altFunc, uint8_t timer, uint8_t channel) {
  PIN_MAP[pin].altFunc = altFunc;
  PIN_MAP[pin].timer = timer;
  PIN_MAP[pin].channel = channel;
}

// TODO: EXTI
void pinMode(uint8_t pin, WiringPinMode wiringMode) {
  // Timer setup
  if (wiringMode == PWM || wiringMode == PWM_IN) {
    // NEW: start the timer automatically
    pinTimerInit(pin);
    
    // Set pin to AF
    uint8_t timer = PIN_MAP[pin].timer;
    uint8_t channel = PIN_MAP[pin].channel;
    pinModeAlt(pin, GPIO_OType_PP, GPIO_PuPd_NOPULL, PIN_MAP[pin].altFunc);

    if (wiringMode == PWM) {
      TIM_OCInitTypeDef TIM_OCInitStructure;
      TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
      TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
      TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

#if defined(SERIES_STM32F30x)
      // had a problem with TIM17 PWM not working without these
      TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
      TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
      TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
      TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;
#endif

      TIM_OCInitStructure.TIM_Pulse = 0;
      switch (channel) {
        case 1:
          TIM_OC1Init(TIMER_MAP[timer].TIMx, &TIM_OCInitStructure);
          TIM_OC1PreloadConfig(TIMER_MAP[timer].TIMx, TIM_OCPreload_Enable);
          break;
        case 2:
          TIM_OC2Init(TIMER_MAP[timer].TIMx, &TIM_OCInitStructure);
          TIM_OC2PreloadConfig(TIMER_MAP[timer].TIMx, TIM_OCPreload_Enable);
          break;
        case 3:
          TIM_OC3Init(TIMER_MAP[timer].TIMx, &TIM_OCInitStructure);
          TIM_OC3PreloadConfig(TIMER_MAP[timer].TIMx, TIM_OCPreload_Enable);
          break;
        case 4:
          TIM_OC4Init(TIMER_MAP[timer].TIMx, &TIM_OCInitStructure);
          TIM_OC4PreloadConfig(TIMER_MAP[timer].TIMx, TIM_OCPreload_Enable);
          break;
        default:
          break;
      }
    } else if (wiringMode == PWM_IN) {
      if (TIMER_MAP[timer].channelData == 0)
        // This timer is not supported for pwmIn
        return;

      TimerChannelData *S = &TIMER_MAP[timer].channelData[channel-1];
      S->bPwmIn = 1;
      S->pin = pin;

      // Enable update interrupt to count rollovers
      TIM_ITConfig(TIMER_MAP[timer].TIMx, TIM_IT_Update, ENABLE);

      // Setup timer input capture
      TIM_ICInitTypeDef TIM_ICInitStructure;
      TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_BothEdge; // Rising/Falling/BothEdge
      TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
      TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
      TIM_ICInitStructure.TIM_ICFilter = 0x5;
      uint16_t TIM_IT = 0;
      switch (channel) {
        case 1:
          TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
          TIM_IT = TIM_IT_CC1;
          break;
        case 2:
          TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;
          TIM_IT = TIM_IT_CC2;
          break;
        case 3:
          TIM_ICInitStructure.TIM_Channel = TIM_Channel_3;
          TIM_IT = TIM_IT_CC3;
          break;
        case 4:
          TIM_ICInitStructure.TIM_Channel = TIM_Channel_4;
          TIM_IT = TIM_IT_CC4;
          break;
      }
      TIM_ICInit(TIMER_MAP[timer].TIMx, &TIM_ICInitStructure);
      // Enable the CCx interrupt
      TIM_ITConfig(TIMER_MAP[timer].TIMx, TIM_IT, ENABLE);
    }
  } else if (wiringMode == PWM_IN_EXTI) {
    // configure pin change interrupt
    EXTIChannel *S = &EXTI_MAP[ PIN_MAP[pin].pin ];
    S->bPwmIn = 1;
    S->pinName = pin;
    // wirishExternalInterruptHandler detects that this is a PWM_IN_EXTI pin, and does not attempt to run the handler, so this can be NULL
    // setting this to high priority, just after SysTick
    uint8_t priority;
    if (PIN_MAP[pin].pin < 5)
      priority = 1;
    else if (PIN_MAP[pin].pin < 10)
      priority = 2;
    else
      priority = 3;
    attachInterruptWithPriority(pin, (ISRType)0, CHANGE, priority);
  } else {
    // Regular GPIO
    // default is no pull
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;

    if (wiringMode == INPUT || wiringMode == INPUT_PULLUP || wiringMode == INPUT_PULLDOWN) {
      GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
      if (wiringMode == INPUT_PULLUP)
        GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
      else if (wiringMode == INPUT_PULLDOWN)
        GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_DOWN;
    } else if (wiringMode == OUTPUT_OPEN_DRAIN || wiringMode == OUTPUT) {
      GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
      if (wiringMode == OUTPUT_OPEN_DRAIN)
        GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;
      else
        GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    } else if (wiringMode == INPUT_ANALOG) {
      GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AN;
    }

    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_Pin = (1 << PIN_MAP[pin].pin);
    GPIO_Init(PIN_MAP[pin].port, &GPIO_InitStruct);
  }
}

// Helper function used by wirish
void pinModeAlt(uint8_t pin, uint32_t oType, uint32_t pull, uint8_t altFunc) {
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStruct.GPIO_OType = oType;
  GPIO_InitStruct.GPIO_PuPd = pull;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStruct.GPIO_Pin = (1 << PIN_MAP[pin].pin);
  // GPIO_InitStruct.Alternate = altFunc;
  GPIO_Init(PIN_MAP[pin].port, &GPIO_InitStruct);

  GPIO_PinAFConfig(PIN_MAP[pin].port, PIN_MAP[pin].pin, altFunc);
}
