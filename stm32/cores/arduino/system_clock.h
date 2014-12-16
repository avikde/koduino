#ifndef _system_clock_
#define _system_clock_

// Uses interrupts and timer 16 for millis() and micros() time keeping
#include <stdint.h>
#include "timer.h"

#ifdef __cplusplus
extern "C"{
#endif // __cplusplus


/// Initialize clock counter and timer hardware.
void systemClockInit();

// Private
void systemClockISR();


/** @addtogroup Time Keeping time
 *  @{
 */

/**
 * @brief 
 * @details Rolls over at max of uint32_t or ~50 days
 * @return Time elapsed in milliseconds since the start of the program
 */
uint32_t millis();

/**
 * @brief 
 * @details Rolls over at max of uint32_t or ~71 minutes
 * @return Time elapsed in microseconds since the start of the program
 */
uint32_t micros();

/**
 * @brief Blocks execution for a specified number of milliseconds
 * @details Only accurate to +/- 1 ms; if you need more precision use delayMicroseconds(). 
 * Note that any code called through [timer interrupts](@ref Timebase) will still execute.
 * 
 * @param nTime Number of milliseconds to wait
 */
void delay(uint32_t nTime);

/**
 * @brief Blocks execution for a specified number of microseconds
 * @details Note that any code called through [timer interrupts](@ref Timebase) will still execute.
 * 
 * @param nTime Number of microseconds to wait
 */
void delayMicroseconds(uint32_t nTime);

/**
 * @brief Non-blocking delay
 * @details Will repeatedly execute the callback function while waiting. Note, it is 
 * preferred to use [timer interrupts](@ref Timebase) for this kind of functionality. 
 * **FIXME:** test if timebase screws up Wire or Serial communication
 * 
 * @param nTime Number of milliseconds to wait
 * @param iterate Function to call while waiting
 */
void delayNB(uint32_t nTime, ISRType iterate);

/** @} */ // end of addtogroup



#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus

#endif
