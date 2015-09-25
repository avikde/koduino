
#ifndef _exti_h_
#define _exti_h_

#include "types.h"

#ifdef __cplusplus
extern "C"{
#endif // __cplusplus

// This function is called by any of the interrupt handlers. It essentially fetches the user function pointer from the array and calls it.
void wirishExternalInterruptHandler(uint8_t EXTI_Line_Number);


/** @addtogroup EXTI External interrupts
 *  @{
 */

/**
 * @brief Enable an interrupt with default priority
 * @details **Note:** There is only ONE EXTI per *pinSource*, i.e. 
 * PA0, PB0 cannot both have interrupts, but PA0 and PB1 can. 
 * Obviously, there can be a total of 16.
 * 
 * @param pinName Name of pin interrupt is connected to
 * @param ISR Callback function to call when interrupt is triggered
 * @param mode One of InterruptTrigger
 */
void attachInterrupt(uint8_t pinName, ISRType ISR, InterruptTrigger mode) {
  attachInterrupt(pinName, ISR, mode, 0x0f);
}

/**
 * @brief Enable an interrupt with a specific priority
 * @details see attachInterrupt()
 * 
 * @param pinName Name of pin interrupt is connected to
 * @param ISR Callback function to call when interrupt is triggered
 * @param mode One of InterruptTrigger
 * @param priority 0x00 is high, 0x0f is low priority
 */
void attachInterrupt(uint8_t pinName, ISRType ISR, InterruptTrigger mode, uint8_t priority);

/**
 * @brief Disable an interrupt
 * @details 
 * 
 * @param pinName Pin name where attachInterrupt() was called
 */
void detachInterrupt(uint8_t pinName);

/**
 * @brief Disable external interrupts
 * @details **Note:** This is unlike the Arduino function which disables ALL interrupts. 
 * That would stop the system clock here as well.
 * Call `NVIC_DisableIRQ()` if you want the arduino behavior
 */
void noInterrupts();

/**
 * @brief Enable external interrupts
 * @details Enabled by default
 */
void interrupts();

/** @} */ // end of addtogroup



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
