#ifndef _timebase_h_
#define _timebase_h_

#include "timer.h"

#ifdef __cplusplus
extern "C"{
#endif // __cplusplus


// cldoc:begin-category(Timebase)

// This is a general timer interrupt class, called as 
// 
// attachTimerInterrupt(i, ISR, freqHz)
// 
// where i = 0,1,2 for user functions.
// i=3 is used for pwmIn
// i=4 is used for the system clock (millis / micros)

// Make up Arduino-like functions
// Minimum freq ~= 0.061Hz, maximum ~= 36MHz)
void attachTimerInterrupt(uint8_t i, ISRType ISR, int freqHz);
void detachTimerInterrupt(uint8_t i);
void noTimerInterrupts();
void timerInterrupts();

// cldoc:end-category()



// cldoc:begin-category(Helper)

// General ISR for all timebase
void timebaseISR(uint8_t i, uint8_t timer);

// Specialized timer interrupt initialization functions
// 
// Uses a basic timer to setup up millisecond interrupts such that the period is 1000
void attachSysClkInterrupt(ISRType ISR);
// Highest resolution for PWM in
void attachHighResClkUpdate(ISRType ISR);

// cldoc:end-category()

#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus

#endif