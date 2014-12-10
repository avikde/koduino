#ifndef Pins_h
#define Pins_h

#include <stdint.h>
#include "types.h"

#ifdef __cplusplus
extern "C"{
#endif // __cplusplus


/** @addtogroup Pins Pin configuration
 *  @{
 */


/**
 * @brief Configure a pin before using it
 * @details This function behaves like the Arduino one, but does more in some cases.
 * 
 * In a few situations, this *MUST* be called in this library when it is not necessary 
 * to in Arduino. For instance, a timer pin must be configured for PWM using 
 * pinMode(pin, PWM) before calling analogWrite() on that pin.
 * 
 * @param pin Pin to configure
 * @param mode One of #WiringPinMode
 */
void pinMode(uint8_t pin, WiringPinMode mode);

/**
 * @brief Low-level function to reassign AF number of timer assigned to a pin
 * @details The default assignments are [here](https://docs.google.com/spreadsheet/pub?key=0Ai-vm-to9OcDdG1zMzR5WFhweGVwNlNnZmtQdlpsb2c&single=true&gid=1&output=html).
 * 
 * @param pin 
 * @param altFunc New AF number (0-15)
 * @param timer New timer (see variant.cpp for the variant you are using to see available timers)
 * @param channel Timer channel (1-4)
 */
void pinRemap(uint8_t pin, uint8_t altFunc, uint8_t timer, uint8_t channel);

/**
 * @brief Low-level function to configure the pin for a specific alternate function. The user should not need to call this directly.
 * @details 
 * 
 * @param pin Pin to configure
 * @param oType One of `GPIO_OType_PP`, `GPIO_OType_OD`
 * @param pull One of `GPIO_PuPd_NOPULL`, `GPIO_PuPd_UP`, `GPIO_PuPd_DOWN`
 * @param altFunc AF number between 0-15 (see datasheet)
 */
void pinModeAlt(uint8_t pin, uint32_t oType, uint32_t pull, uint8_t altFunc);

/** @} */

#ifdef __cplusplus
} // extern "C"
#endif

#endif
