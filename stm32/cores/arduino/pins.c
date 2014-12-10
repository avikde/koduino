
#include "pins.h"
#include "timer.h"

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
  if (wiringMode == PWM || wiringMode == PULSE_IN) {
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
    } else if (wiringMode == PULSE_IN) {
      if (TIMER_MAP[timer].channelData == 0)
        // This timer is not supported for pulseIn
        return;

      TimerChannelData *S = &TIMER_MAP[timer].channelData[channel-1];
      S->bPulseIn = 1;
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
  }

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
