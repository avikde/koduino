
#include "Encoder.h"
#include "pins.h"

void Encoder::init(uint8_t timer, uint16_t maxCount, uint8_t pin1, uint8_t af1, uint8_t pin2, uint8_t af2) {
  this->timer = timer;

  // Alternate function, pull-up
  pinModeAlt(pin1, GPIO_OType_PP, GPIO_PuPd_UP, af1);
  pinModeAlt(pin2, GPIO_OType_PP, GPIO_PuPd_UP, af2);

  // Start timer
  timerInitHelper(timer, 0, maxCount);

  // Configure input capture
  TIM_ICInitTypeDef TIM_ICInitStruct;
  // filter is between 0x0 and 0xF, 0 for no filter, 0xF for a lot
  TIM_ICInitStruct.TIM_ICFilter = 0x0;
  TIM_ICInitStruct.TIM_Channel = TIM_Channel_1;
  TIM_ICInit(TIMER_MAP[timer].TIMx, &TIM_ICInitStruct);
  TIM_ICInitStruct.TIM_Channel = TIM_Channel_2;
  TIM_ICInit(TIMER_MAP[timer].TIMx, &TIM_ICInitStruct);
  TIM_EncoderInterfaceConfig(TIMER_MAP[timer].TIMx, TIM_EncoderMode_TI12, TIM_ICPolarity_Falling, TIM_ICPolarity_Falling);
  
  // Enable
  TIM_Cmd(TIMER_MAP[timer].TIMx, ENABLE);
}

// void Encoder::init(TimerName timer, uint16_t maxCount, const TimerPin& p1, const TimerPin& p2) {
//   init(timer, maxCount, p1.pin, p1.af, p2.pin, p2.af);
// }

