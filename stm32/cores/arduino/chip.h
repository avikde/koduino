/**
 * @authors Avik De <avikde@gmail.com>

  This file is part of koduino <https://github.com/avikde/koduino>

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation, either
  version 3 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, see <http://www.gnu.org/licenses/>.
 */
  
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
