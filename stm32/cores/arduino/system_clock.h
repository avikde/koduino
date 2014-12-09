#ifndef _system_clock_
#define _system_clock_

// Uses interrupts and timer 16 for millis() and micros() time keeping
#include <stdint.h>
#include "timer.h"

#ifdef __cplusplus
extern "C"{
#endif // __cplusplus


// cldoc:begin-category(Helper)

/// Initialize clock counter and timer hardware.
void systemClockInit();

// Private
void systemClockISR();

// cldoc:end-category()



// cldoc:begin-category(Time)

uint32_t millis();
uint32_t micros();

void delay(uint32_t nTime);
void delayMicroseconds(uint32_t nTime);

// Non-blocking delay which calls the specified function, but unlike timebase it doesn't use interrupts, so it won't screw up communication
typedef void (*IterFunc)(void);
void delayNB(uint32_t nTime, IterFunc iterate);

// cldoc:end-category()



#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus

#endif
