#ifndef Pins_h
#define Pins_h

#include <stdint.h>
#include "types.h"

#ifdef __cplusplus
extern "C"{
#endif // __cplusplus

// cldoc:begin-category(Pins)

// Configure a pin before using it
// @name Pin to configure
// @mode One of <PinMode>
// 
// This function behaves like the Arduino one, but does more in some cases.
// 
// In a few situations, this *MUST* be called in this library when it is not necessary to in Arduino. For instance, a timer pin must be configured for PWM using <pinMode> with a <TimerPinName> argument before calling <analogWrite> on that pin.
void pinMode(uint8_t pin, WiringPinMode mode);

// Use this to reassign the timer (defaults are those of mainboard_v1.1)
void pinRemap(uint8_t pin, uint8_t altFunc, uint8_t timer, uint8_t channel);

// Low-level function to configure the pin for a specific alternate function. The user should not need to call this directly.
// See the datasheet for AF numbers
void pinModeAlt(uint8_t pin, uint32_t oType, uint32_t pull, uint8_t altFunc);

// cldoc:end-category()

#ifdef __cplusplus
} // extern "C"
#endif

#endif
