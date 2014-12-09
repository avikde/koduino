
#ifndef _exti_h_
#define _exti_h_

// #include "types.h"
#include "timer.h"

#ifdef __cplusplus
extern "C"{
#endif // __cplusplus

// cldoc:begin-category(Helper)

// Interrupt types
typedef enum InterruptTrigger {
  RISING, FALLING, CHANGE
} InterruptTrigger;

// This function is called by any of the interrupt handlers. It essentially fetches the user function pointer from the array and calls it.
void wirishExternalInterruptHandler(uint8_t EXTI_Line_Number);

// cldoc:end-category()



// cldoc:begin-category(Interrupts)

// Enable an interrupt
// @pinName Name of pin interrupt is connected to
// @ISR Callback function to call when interrupt is triggered
// @mode One of RISING, FALLING, CHANGE
// 
// Enable an interrupt. **Note:** there is only ONE EXTI per *pinSource*, i.e. PA0, PB0 cannot both have interrupts, but PA0 and PB1 can. Obviously, there can be a total of 16.
void attachInterrupt(uint8_t pinName, ISRType ISR, InterruptTrigger mode);


// Disable an interrupt
// @pinName Pin name where <attachInterrupt> was called
void detachInterrupt(uint8_t pinName);


// Disable external interrupts
// 
// Disable external interrupts. **Note:** This is unlike the Arduino function which disables ALL interrupts. That would stop the system clock here as well. (NVIC_DisableIRQ() would do that).
void noInterrupts();

// Enable external interrupts
void interrupts();

// cldoc:end-category()




// // PWM Input using EXTI
// float pwmIn(uint8_t pinName);

// // Private
// void pinModePwmIn(uint8_t pinName);
// void pwmInClkInit();
// uint32_t pwmInTicks();

#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus

#endif
