
#ifndef chip_h
#define chip_h

// Include the CMSIS, HAL based on chip
#if defined(SERIES_STM32F37x)
#include "stm32f37x.h"
#elif defined(SERIES_STM32F30x)
#include "stm32f30x.h"
#elif defined(SERIES_STM32F4xx)
#include "stm32f4xx.h"
#elif defined(SERIES_STM32F10x)
#include "stm32f10x.h"
#endif

// #define assert_param(expr) ((expr) ? (void)0 : assert_failed((uint8_t *)__FILE__, __LINE__))

#endif
