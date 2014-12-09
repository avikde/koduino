
#ifndef chip_h
#define chip_h

// Include the CMSIS, HAL based on chip
#if defined(STM32F37x)
#include "stm32f37x.h"
#elif defined(STM32F40_41xxx)
#include "stm32f4xx.h"
#endif

// #define assert_param(expr) ((expr) ? (void)0 : assert_failed((uint8_t *)__FILE__, __LINE__))

#endif
