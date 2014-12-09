#ifndef nvic_h
#define nvic_h

#include "chip.h"

#ifdef __cplusplus
extern "C"{
#endif // __cplusplus


// cldoc:begin-category(Helper)

void nvicEnable(uint8_t irqChannel, uint8_t priority);

// cldoc:end-category()

#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus

#endif

