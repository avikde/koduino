
#ifndef Arduino_h
#define Arduino_h

#ifdef __cplusplus
extern "C"{
#endif // __cplusplus

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define clockCyclesPerMicrosecond() ( SystemCoreClock / 1000000L )
#define clockCyclesToMicroseconds(a) ( ((a) * 1000L) / (SystemCoreClock / 1000L) )
#define microsecondsToClockCycles(a) ( (a) * (SystemCoreClock / 1000000L) )

void yield(void);

// sketch
extern void setup();
extern void loop();


#include "wiring_constants.h"
#include "gpio.h"
#include "timer.h"
#include "system_clock.h"
#include "timebase.h"
#include "adc.h"
#include "pins.h"
#include "exti.h"

#include <arm_math.h>

#ifdef __cplusplus
} // extern "C"


#include "variant.h"

// #include "WCharacter.h"
#include "WString.h"
// #include "Tone.h"
#include "WMath.h"
#include "USARTClass.h"
// Streaming library http://arduiniana.org/libraries/streaming/
#include "Streaming.h"

#endif // __cplusplus

#endif // Arduino_h
