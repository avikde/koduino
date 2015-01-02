
#include "exti.h"
#include "nvic.h"
#include "system_clock.h"
#include "gpio.h"
#include "timebase.h"
#include "variant.h"

// Some code from https://github.com/spark/core-firmware/blob/master/src/spark_wiring_interrupts.cpp

// Indexed by the pinSource of the pin
const uint32_t extiLines[] = {
  EXTI_Line0, 
  EXTI_Line1, 
  EXTI_Line2, 
  EXTI_Line3, 
  EXTI_Line4, 
  EXTI_Line5, 
  EXTI_Line6, 
  EXTI_Line7, 
  EXTI_Line8, 
  EXTI_Line9, 
  EXTI_Line10, 
  EXTI_Line11, 
  EXTI_Line12, 
  EXTI_Line13, 
  EXTI_Line14, 
  EXTI_Line15
};

const uint8_t extiIRQn[] = {
  EXTI0_IRQn,     //0
  EXTI1_IRQn,     //1
#if defined(STM32F37x)
  EXTI2_TS_IRQn,  //2
#else
  EXTI2_IRQn,     //2
#endif
  EXTI3_IRQn,     //3
  EXTI4_IRQn,     //4
  EXTI9_5_IRQn,   //5
  EXTI9_5_IRQn,   //6
  EXTI9_5_IRQn,   //7
  EXTI9_5_IRQn,   //8
  EXTI9_5_IRQn,   //9
  EXTI15_10_IRQn, //10
  EXTI15_10_IRQn, //11
  EXTI15_10_IRQn, //12
  EXTI15_10_IRQn, //13
  EXTI15_10_IRQn, //14
  EXTI15_10_IRQn  //15
};

// Create a structure for user ISR function pointers
typedef struct {
  void (*handler)();
  uint8_t bPwmIn;
  volatile int risingedge, pulsewidth, period;
  volatile uint8_t pinName;
} EXTIChannel;

//Array to hold user ISR function pointers
static EXTIChannel extiChannels[] = {
  { .handler = (ISRType)0, .bPwmIn = 0, .risingedge = 0, .pulsewidth = 0, .period = 0 },  // EXTI0
  { .handler = (ISRType)0, .bPwmIn = 0, .risingedge = 0, .pulsewidth = 0, .period = 0 },  // EXTI1
  { .handler = (ISRType)0, .bPwmIn = 0, .risingedge = 0, .pulsewidth = 0, .period = 0 },  // EXTI2
  { .handler = (ISRType)0, .bPwmIn = 0, .risingedge = 0, .pulsewidth = 0, .period = 0 },  // EXTI3
  { .handler = (ISRType)0, .bPwmIn = 0, .risingedge = 0, .pulsewidth = 0, .period = 0 },  // EXTI4
  { .handler = (ISRType)0, .bPwmIn = 0, .risingedge = 0, .pulsewidth = 0, .period = 0 },  // EXTI5
  { .handler = (ISRType)0, .bPwmIn = 0, .risingedge = 0, .pulsewidth = 0, .period = 0 },  // EXTI6
  { .handler = (ISRType)0, .bPwmIn = 0, .risingedge = 0, .pulsewidth = 0, .period = 0 },  // EXTI7
  { .handler = (ISRType)0, .bPwmIn = 0, .risingedge = 0, .pulsewidth = 0, .period = 0 },  // EXTI8
  { .handler = (ISRType)0, .bPwmIn = 0, .risingedge = 0, .pulsewidth = 0, .period = 0 },  // EXTI9
  { .handler = (ISRType)0, .bPwmIn = 0, .risingedge = 0, .pulsewidth = 0, .period = 0 },  // EXTI10
  { .handler = (ISRType)0, .bPwmIn = 0, .risingedge = 0, .pulsewidth = 0, .period = 0 },  // EXTI11
  { .handler = (ISRType)0, .bPwmIn = 0, .risingedge = 0, .pulsewidth = 0, .period = 0 },  // EXTI12
  { .handler = (ISRType)0, .bPwmIn = 0, .risingedge = 0, .pulsewidth = 0, .period = 0 },  // EXTI13
  { .handler = (ISRType)0, .bPwmIn = 0, .risingedge = 0, .pulsewidth = 0, .period = 0 },  // EXTI14
  { .handler = (ISRType)0, .bPwmIn = 0, .risingedge = 0, .pulsewidth = 0, .period = 0 },  // EXTI15
};

void attachInterrupt(uint8_t pinName, ISRType ISR, InterruptTrigger mode) {
  // Connect EXTIi to this pin
  const PinInfo *pinfo = &PIN_MAP[pinName];
  if (pinfo->port == GPIOA)
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, pinfo->pin);
  else if (pinfo->port == GPIOB)
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, pinfo->pin);
  else if (pinfo->port == GPIOC)
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, pinfo->pin);
  else if (pinfo->port == GPIOD)
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOD, pinfo->pin);
  else if (pinfo->port == GPIOE)
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, pinfo->pin);
  else if (pinfo->port == GPIOF)
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOF, pinfo->pin);

  // Configure EXTIi line
  EXTI_InitTypeDef EXTI_InitStructure;

  EXTI_InitStructure.EXTI_Line = extiLines[pinfo->pin];
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  if (mode == RISING)
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  else if (mode == FALLING)
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  else
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

  // Local code
  extiChannels[pinfo->pin].handler = ISR;

  // Enable the interrupt with low priority.
  // TODO: check https://github.com/spark/core-firmware/blob/master/src/spark_wiring_interrupts.cpp does something peculiar with special cases??
  nvicEnable(extiIRQn[pinfo->pin], 0x0f);
}

void detachInterrupt(uint8_t pinName) {
  const PinInfo *pinfo = &PIN_MAP[pinName];
  //Clear the pending interrupt flag for that interrupt pin
  EXTI_ClearITPendingBit(extiLines[pinfo->pin]);

  //EXTI structure to init EXT
  EXTI_InitTypeDef EXTI_InitStructure;
  //Select the appropriate EXTI line
  EXTI_InitStructure.EXTI_Line = extiLines[pinfo->pin];
  //disable that EXTI line
  EXTI_InitStructure.EXTI_LineCmd = DISABLE;
  //send values to registers
  EXTI_Init(&EXTI_InitStructure);

  //unregister the user's handler
  extiChannels[pinfo->pin].handler = (ISRType)0;
}

void noInterrupts() {
  //Only disable the interrupts that are exposed to the user
  for (uint8_t i=0; i<16; ++i)
    NVIC_DisableIRQ(extiIRQn[i]);
}

void interrupts() {
  //Only disable the interrupts that are exposed to the user
  for (uint8_t i=0; i<16; ++i)
    NVIC_EnableIRQ(extiIRQn[i]);
}

void wirishExternalInterruptHandler(uint8_t EXTI_Line_Number) {
  EXTIChannel *S = &extiChannels[EXTI_Line_Number];
  // // pwmIn pin?
  // if (S->bPwmIn == 1) {
  //   // uint32_t current = micros();
  //   // if (digitalRead(S->pinName)) {
  //   //   // This was a rising edge
  //   //   S->period = current - S->risingedge;
  //   //   S->risingedge = current;
  //   // } else {
  //   //   // This was a falling edge
  //   //   S->pulsewidth = current - S->risingedge;
  //   //   if (S->pulsewidth < 0)
  //   //     S->pulsewidth += S->period;
  //   // }
  //   uint32_t current = pwmInTicks();
  //   // Check for zeros?
  //   if (current == S->risingedge) return;

  //   if (digitalRead(S->pinName)) {
  //     // This was a rising edge
  //     S->period = current - S->risingedge;
  //     if (S->period < 0) S->period += 65536;
  //     if (S->period > 65535) S->period -= 65536;
  //     S->risingedge = current;
  //   } else {
  //     // This was a falling edge
  //     S->pulsewidth = current - S->risingedge;
  //     if (S->pulsewidth < 0) S->pulsewidth += 65536;
  //     if (S->pulsewidth > 65535) S->pulsewidth -= 65536;
  //   }
  // } else {
    // ELSE fetch the user function pointer from the array
    ISRType handler = S->handler;
    //Check to see if the user handle is NULL
    if (handler != 0)
      handler();
  // }
}

// ===========================================================================

// // Global
// volatile uint32_t _pwmInClkRollovers = 0;

// // Private timekeeping (like system clock, but higher resolution)
// // Each tick is 0.25us
// void pwmInClkISR() {
//   _pwmInClkRollovers++;
// }

// void pwmInClkInit() {
//   attachHighResClkUpdate(pwmInClkISR);
// }

// uint32_t pwmInTicks() {
//   return _pwmInClkRollovers * 65536 + TIM_GetCounter(TIM17);
// }

// // Public API
// void pinModePwmIn(uint8_t pinName) {
//   EXTIChannel *S = &extiChannels[ PIN_MAP[pinName].pin ];
//   S->bPwmIn = 1;
//   S->pinName = pinName;
//   attachInterrupt(pinName, (ISRType)0, CHANGE);
// }

// // This is the function the user should call
// float pwmIn(uint8_t pinName) {
//   EXTIChannel *S = &extiChannels[ PIN_MAP[pinName].pin ];

//   // Pin configured for pwmIn?
//   if (S->pinName != pinName)
//     pinModePwmIn(pinName);

//   // HACK: need to detect if the signal goes flat. 5ms = 5000us = 200000 ticks
//   static int timediff;
//   timediff = pwmInTicks() - S->risingedge;
//   if (timediff < 0) timediff += 65536;
//   if (timediff > 65535) timediff -= 65536;
//   if (timediff > 20000)
//     return 0;

//   return (S->period > 0) ? S->pulsewidth/(float)S->period : 0;
//   // return S->pulsewidth/(float)S->period;
//   // return S->period;
// }


