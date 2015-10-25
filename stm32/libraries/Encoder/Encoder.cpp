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
#include "Encoder.h"
#include "pins.h"

void Encoder::init(uint8_t pin1, uint8_t pin2, uint16_t maxCount) {
  // NEW: read from pin config.
  timer = PIN_MAP[pin1].timer;
  pinModeAlt(pin1, GPIO_OType_PP, GPIO_PuPd_UP, PIN_MAP[pin1].altFunc);
  pinModeAlt(pin2, GPIO_OType_PP, GPIO_PuPd_UP, PIN_MAP[pin2].altFunc);

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


// // BEFORE
// void Encoder::init(uint8_t timer, uint16_t maxCount, uint8_t pin1, uint8_t af1, uint8_t pin2, uint8_t af2) {
// this->timer = timer;
// pinModeAlt(pin1, GPIO_OType_PP, GPIO_PuPd_UP, af1);
// pinModeAlt(pin2, GPIO_OType_PP, GPIO_PuPd_UP, af2);
// ...

