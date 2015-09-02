
#ifndef _timer_h_
#define _timer_h_

#include "chip.h"
#include "types.h"

#ifdef __cplusplus
extern "C"{
#endif // __cplusplus

// FUTURE IDEAS?
// 
// pinModeTimer should take another argument: ENCODER_AB, PWM_IN or PWM_OUT
// if pwmIn is called and it is in output mode it should be re-inited TIM_ICInit etc. <- function timerInitPulseInPin
// if analogWrite is called and it is in input mode it should be re-inited TIM_OC1Init etc. <- timerInitPWMPin


/** @addtogroup PWM PWM output and input
 *  @{
 */

void analogWriteResolution(uint8_t nbits);

/**
 * @brief Assign a PWM frequency to the specified pin (default = 1000 Hz)
 * @details Please see the [pin remapping](@ref Pins) documentation for more details about 
 * connected timers to pins, including defaults. This function will change the PWM frequency for
 * all pins connected to the same timer.
 * 
 * *Gory details:*
 * * There is a 36MHz base clock.
 * * Resolution: When PWM freq = 20KHz, resolution = 36000/20 = 1800 ~= 11 bits, and better for lower frequencies
 * * Minimum freuency is 36000000/65535 = 549Hz
 * 
 * @param pin Pin to change frequency on
 * @param freqHz New frequency in Hz (default is 1000)
 */
void analogWriteFrequency(uint8_t pin, int freqHz);

/**
 * @brief Change the PWM duty cycle on a pin
 * @details Unlike Arduino, pinMode() must have been called on the pin beforehand with `PWM`.
 * Also, the duty cycle is specified in floating point, and the resolution is only limited by 
 * the timer (see analogWriteFrequency()).
 * 
 * @param pin 
 * @param duty The new duty cycle in normalized floating point from 0.0 (0%) to 1.0 (100%).
 */
void analogWriteFloat(uint8_t pin, float duty);

// Not the most efficient: uses analogWriteFloat
void analogWrite(uint8_t pin, uint32_t duty);

/**
 * @brief Read a PWM signal (non-blocking)
 * @details pinMode() with `PWM_IN` must have been called on the pin before-hand.
 * Unlike Arduino, this function does not block and wait. It uses high-speed timer interrupts 
 * to catch rising and falling edges, and keeps track of the time when these happen to 
 * estimate the PWM duty cycle. 
 * 
 * *Advanced:*
 * 
 * * Period: The period of the PWM is also available in low-level form. The command 
 * ~~~
 * TIMER_MAP[ PIN_MAP[ *pin* ].timer ].channelData[ PIN_MAP[ *pin* ].channel-1].period
 * ~~~
 * will return the PWM period in units such that 36000 -> 1ms, 3600 -> 0.1ms etc.
 * 
 * @param pin Pin to read
 * @return A float from 0.0 to 1.0 corresponding to PWM duty cycle
 */
float pwmIn(uint8_t pin);

// To try and reduce position reading bugs, fix pwmIn period
// 36000 -> 1ms (1KHz PWM)
// Set = 0 to autodetect period
extern int PWM_IN_FIXED_PERIOD;

// Helper functions
void pwmInRaw(uint8_t name, int *period, int *pulseWidth);

/** @} */ // end of addtogroup

void timerInit(uint8_t timer, int freqHz);

void pinTimerInit(uint8_t pin);

// Timer init with period: mostly a helper function
void timerInitHelper(uint8_t timer, uint16_t prescaler, uint32_t period);

// Main GP timer ISR
void timerISR(uint8_t timer);


// Special timer features
void complementaryPWM(uint8_t timer, uint16_t deadtime);


#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus

#endif

