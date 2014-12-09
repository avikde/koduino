
#include "variant.h"
#include "types.h"
#include "USARTClass.h"

// Pins
const uint8_t PA0 = 0;
const uint8_t PA1 = 1;
const uint8_t PA2 = 2;
const uint8_t PA3 = 3;
const uint8_t PA4 = 4;
const uint8_t PA5 = 5;
const uint8_t PA6 = 6;
const uint8_t PA8 = 7;
const uint8_t PA9 = 8;
const uint8_t PA10 = 9;
const uint8_t PA11 = 10;
const uint8_t PA12 = 11;
const uint8_t PA13 = 12;
const uint8_t PA14 = 13;
const uint8_t PA15 = 14;
const uint8_t PB0 = 15;
const uint8_t PB1 = 16;
const uint8_t PB2 = 17;
const uint8_t PB3 = 18;
const uint8_t PB4 = 19;
const uint8_t PB5 = 20;
const uint8_t PB6 = 21;
const uint8_t PB7 = 22;
const uint8_t PB8 = 23;
const uint8_t PB9 = 24;
const uint8_t PB14 = 25;
const uint8_t PB15 = 26;
const uint8_t PC13 = 27;
const uint8_t PC14 = 28;
const uint8_t PC15 = 29;
const uint8_t PD8 = 30;
const uint8_t PE8 = 31;
const uint8_t PE9 = 32;
const uint8_t PF1 = 33;
const uint8_t PF6 = 34;
const uint8_t PF7 = 35;

// Timers

// General purpose 4 channels 32-bit
const uint8_t TIMER2 = 0;
const uint8_t TIMER5 = 1;
// General purpose 4 channels 16-bit
const uint8_t TIMER3 = 2;
const uint8_t TIMER4 = 3;
const uint8_t TIMER19 = 4;
// General purpose 2 channels 16-bit
const uint8_t TIMER12 = 5;
const uint8_t TIMER15 = 6;
// General purpose 1 channel 16-bit
const uint8_t TIMER13 = 7;
const uint8_t TIMER14 = 8;
const uint8_t TIMER16 = 9;
const uint8_t TIMER17 = 10;
// Basic 0 channels
const uint8_t TIMER6 = 11;
const uint8_t TIMER7 = 12;
const uint8_t TIMER18 = 13;


// Constant pin info
PinInfo PIN_MAP[] = {
  {GPIOA, 0, 0, 2, TIMER5, 1, NOT_SET},//0
  {GPIOA, 1, 1, 1, TIMER2, 2, NOT_SET},//1
  {GPIOA, 2, 2, 2, TIMER5, 3, NOT_SET},//2
  {GPIOA, 3, 3, 2, TIMER5, 4, NOT_SET},//3
  {GPIOA, 4, 4, 0, NOT_SET, NOT_SET, NOT_SET},//4
  {GPIOA, 5, 5, 1, TIMER2, 1, NOT_SET},//5
  {GPIOA, 6, 6, 2, TIMER3, 1, NOT_SET},//6
  {GPIOA, 8, NOT_SET, 0, NOT_SET, NOT_SET, NOT_SET},//7 SPI2_SCK@5
  {GPIOA, 9, NOT_SET, 7, NOT_SET, NOT_SET, NOT_SET},//8 USART1_TX
  {GPIOA, 10, NOT_SET, 7, NOT_SET, NOT_SET, NOT_SET},//9 USART1_RX
  {GPIOA, 11, NOT_SET, 2, TIMER5, 2, NOT_SET},//10
  {GPIOA, 12, NOT_SET, 6, NOT_SET, NOT_SET, NOT_SET},//11 SPI_SCK
  {GPIOA, 13, NOT_SET, 6, NOT_SET, NOT_SET, NOT_SET},//12 SPI1_MISO
  {GPIOA, 14, NOT_SET, 10, TIMER12, 1, NOT_SET},//13 I2C1_SDA@4
  {GPIOA, 15, NOT_SET, 10, TIMER12, 2, NOT_SET},//14 I2C1_SCL@4
  
  {GPIOB, 0, 8, 2, TIMER3, 3, NOT_SET},//15
  {GPIOB, 1, 9, 2, TIMER3, 4, NOT_SET},//16
  {GPIOB, 2, NOT_SET, NOT_SET, NOT_SET, NOT_SET, NOT_SET},//17
  {GPIOB, 3, NOT_SET, 1, TIMER2, 2, NOT_SET},//18 USART2_TX
  {GPIOB, 4, NOT_SET, 7, NOT_SET, NOT_SET, NOT_SET},//19 USART2_RX
  {GPIOB, 5, NOT_SET, 2, TIMER3, 2, NOT_SET},//20
  {GPIOB, 6, NOT_SET, 2, TIMER4, 1, NOT_SET},//21
  {GPIOB, 7, NOT_SET, 2, TIMER4, 2, NOT_SET},//22
  {GPIOB, 8, NOT_SET, 2, TIMER4, 3, NOT_SET},//23 USART3_TX@7
  {GPIOB, 9, NOT_SET, 2, TIMER4, 4, NOT_SET},//24 USART3_RX@7
  {GPIOB, 14, NOT_SET, 1, TIMER15, 1, NOT_SET},//25 SPI2_MISO@5
  {GPIOB, 15, NOT_SET, 1, TIMER15, 2, NOT_SET},//26 SPI2_MOSI@5
  
  {GPIOC, 13, NOT_SET, NOT_SET, NOT_SET, NOT_SET, NOT_SET},//27
  {GPIOC, 14, NOT_SET, NOT_SET, NOT_SET, NOT_SET, NOT_SET},//28
  {GPIOC, 15, NOT_SET, NOT_SET, NOT_SET, NOT_SET, NOT_SET},//29
  
  {GPIOD, 8, NOT_SET, NOT_SET, NOT_SET, NOT_SET, NOT_SET},//30
  
  {GPIOE, 8, NOT_SET, NOT_SET, NOT_SET, NOT_SET, NOT_SET},//31
  {GPIOE, 9, NOT_SET, NOT_SET, NOT_SET, NOT_SET, NOT_SET},//32
  
  {GPIOF, 1, NOT_SET, 4, NOT_SET, NOT_SET, NOT_SET},//33 I2C2_SCL
  {GPIOF, 6, NOT_SET, 5, NOT_SET, NOT_SET, NOT_SET},//34 SPI1_MOSI
  {GPIOF, 7, NOT_SET, 4, NOT_SET, NOT_SET, NOT_SET}//35 I2C2_SDA
};

// Run-time timer configuration
TimerChannelData timer2ch[4] = {
  {0, false, 0, 0, 0, 0},
  {0, false, 0, 0, 0, 0},
  {0, false, 0, 0, 0, 0},
  {0, false, 0, 0, 0, 0}
};
TimerChannelData timer3ch[4] = {
  {0, false, 0, 0, 0, 0},
  {0, false, 0, 0, 0, 0},
  {0, false, 0, 0, 0, 0},
  {0, false, 0, 0, 0, 0}
};
TimerChannelData timer4ch[4] = {
  {0, false, 0, 0, 0, 0},
  {0, false, 0, 0, 0, 0},
  {0, false, 0, 0, 0, 0},
  {0, false, 0, 0, 0, 0}
};
TimerChannelData timer5ch[4] = {
  {0, false, 0, 0, 0, 0},
  {0, false, 0, 0, 0, 0},
  {0, false, 0, 0, 0, 0},
  {0, false, 0, 0, 0, 0}
};
TimerChannelData timer12ch[2] = {
  {0, false, 0, 0, 0, 0},
  {0, false, 0, 0, 0, 0}
};
TimerChannelData timer15ch[2] = {
  {0, false, 0, 0, 0, 0},
  {0, false, 0, 0, 0, 0}
};
TimerChannelData timer19ch[4] = {
  {0, false, 0, 0, 0, 0},
  {0, false, 0, 0, 0, 0},
  {0, false, 0, 0, 0, 0},
  {0, false, 0, 0, 0, 0}
};

// Constant timer info
TimerInfo TIMER_MAP[] = {
  {TIM2, TIM2_IRQn, 0, timer2ch},
  {TIM5, TIM5_IRQn, 0, timer5ch},
  {TIM3, TIM3_IRQn, 0, timer3ch},
  {TIM4, TIM4_IRQn, 0, timer4ch},
  {TIM19, TIM19_IRQn, 0, timer19ch},

  {TIM12, TIM12_IRQn, 0, timer12ch},
  {TIM15, TIM15_IRQn, 0, timer15ch},
  {TIM13, TIM13_IRQn, 0, (TimerChannelData *)0},
  {TIM14, TIM14_IRQn, 0, (TimerChannelData *)0},
  {TIM16, TIM16_IRQn, 0, (TimerChannelData *)0},
  {TIM17, TIM17_IRQn, 0, (TimerChannelData *)0},

  {TIM6, TIM6_DAC1_IRQn, 0, (TimerChannelData *)0},
  {TIM7, TIM7_IRQn, 0, (TimerChannelData *)0},
  {TIM18, TIM18_DAC2_IRQn, 0, (TimerChannelData *)0}
};

// Use the 3 basic timers and two others
TimebaseChannel TIMEBASE_MAP[] = {
  {.timer = NOT_SET, .isr = 0}, // 0:user
  {.timer = NOT_SET, .isr = 0}, // 1:user
  {.timer = NOT_SET, .isr = 0}, // 2:user
  {.timer = NOT_SET, .isr = 0}, // 3:pwmIn
  {.timer = NOT_SET, .isr = 0} // 4:System clock, i.e. millis
};

// Peripherals
USARTInfo USART_MAP[3] = {
  { USART1, USART1_IRQn, PA9, PA10 },
  { USART2, USART2_IRQn, PB3, PB4 },
  { USART3, USART3_IRQn, PB8, PB9 },
};
USARTClass Serial1(&USART_MAP[0]);
USARTClass Serial2(&USART_MAP[1]);
USARTClass Serial3(&USART_MAP[2]);



bool isTimer32Bit(uint8_t tim) {
  return (tim < TIMER3);
}

uint8_t numChannelsInTimer(uint8_t tim) {
  if (tim < TIMER12)
    return 4;
  else if (tim < TIMER13)
    return 2;
  else if (tim < TIMER6)
    return 1;
  else
    return 0;
}

bool isAnalogPin(uint8_t pin) {
  return (PIN_MAP[pin].adcChannel != NOT_SET);
}

void variantInit() {
  TIMEBASE_MAP[0].timer = TIMER6;
  TIMEBASE_MAP[1].timer = TIMER7;
  TIMEBASE_MAP[2].timer = TIMER16;
  TIMEBASE_MAP[3].timer = TIMER17;
  TIMEBASE_MAP[4].timer = TIMER18;

  // GPIO
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOD, ENABLE);
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOE, ENABLE);
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOF, ENABLE);
  // SYSCFG - needed for EXTI
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

  // Timer clocks
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM12, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM13, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM14, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM15, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM16, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM17, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM18, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM19, ENABLE);

  // USART
  RCC_USARTCLKConfig(RCC_USART1CLK_HSI);
  RCC_USARTCLKConfig(RCC_USART2CLK_HSI);
  RCC_USARTCLKConfig(RCC_USART3CLK_HSI);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
}
