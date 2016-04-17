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
#ifndef types_h
#define types_h

#include <stdint.h>
#include "chip.h"

#ifdef __cplusplus
extern "C"{
#endif // __cplusplus

// Types needed by variant.h are in here
#define NOT_SET 0xff

// For the ioConfig bitmask
#define IOCFGBIT_ACTIVELOW  0 //
#define IOCFGBIT_BOARDLED   1 //
#define IOCFGBIT_PWM        2
// #define IOCFGBIT_PIN_MODE   2 //
// #define 
// 0 => Digital, 1=> Timer PWM, 2=> Timer pulsein, 3=> Timer encoder, 4=>Analog input, 5=> EXTI, 6=>USART, 7=>I2C

typedef struct PinInfo {
  // GPIO port
  GPIO_TypeDef * const port;
  // GPIO pinSource
  const uint8_t pin;
  // ADC channel (if it is not an analog pin, adcChannel is set to 0xFF)
  // FIXME if there are multiple ADC's make this a mux? lower nibble will be channel (0-15)
  //  and upper nibble will be binary ADC4|ADC3|ADC2|ADC1. E.g. if a pin is on ADC1, ADC2, 
  // and channel 1,
  // this will be 00110001
  // Currently the upper nibble is unused
  const uint8_t adcChannel;
  // Alternate function
  uint8_t altFunc;
  // Timer name (if this pin is connected to a timer)
  uint8_t timer;
  // Time channel (if this pin is connected to a timer)
  uint8_t channel;
  // Index into the array of ADC conversions (not all channels may be activated)
  uint8_t adcConvIndex;
  // Bitmask, where 
  // bit 0 = (0=>active high, 1=>active low) only for LED
  // bit 1 = (0=>general pin, 1=>board LED)
  // bit 2 = ?
  uint8_t ioConfig;
} __attribute__ ((packed)) PinInfo;


// Timer channel configuration - changes at runtime
typedef struct TimerChannelData {
  // Reference to the pin attached to this channel (for analogWrite, etc.)
  uint8_t pin;
  // PulseIn
  uint8_t bPwmIn;
  volatile int risingEdge, pulseWidth, period;
  volatile uint32_t lastRollover;
} TimerChannelData;

// Timer info
typedef struct TimerInfo {
  TIM_TypeDef * const TIMx;
  const IRQn_Type IRQn;
  // non-constant
  // frequency
  int freqHz;
  // how many times rolled over (for pwmIn)
  volatile uint32_t numRollovers;
  // variable number of channels; assigned in variant.cpp
  TimerChannelData *channelData;
} TimerInfo;

// pinMode
typedef enum WiringPinMode {
  OUTPUT, OUTPUT_OPEN_DRAIN,
  INPUT, INPUT_ANALOG, INPUT_PULLUP, INPUT_PULLDOWN, 
  PWM, 
  PWM_IN, PWM_IN_EXTI
} WiringPinMode;

// Logic levels
typedef enum LogicValue {
  LOW = 0, HIGH = 1, TOGGLE = 2
} LogicValue;

// General prototype for a user callback (like from timer or external interrupts)
typedef void (*ISRType)(void);

// Timebase
typedef struct {
  uint8_t timer;
  ISRType isr;
} __attribute__ ((packed)) TimebaseChannel;

// USART
#define SERIAL_BUFFER_SIZE 64

typedef struct RingBuffer {
  uint8_t buffer[SERIAL_BUFFER_SIZE];
  volatile uint8_t head;
  volatile uint8_t tail;
} RingBuffer;

typedef void (*ByteFunc)(uint8_t);

typedef struct USARTInfo {
  // STM32 stuff
  USART_TypeDef *USARTx;
  IRQn_Type irqChannel;
  uint8_t txPin, rxPin;

  // Buffer pointers. These need to be global for IRQ handler access
  RingBuffer* txBuf;
  RingBuffer* rxBuf;

  // for callback on RX
  ByteFunc rxCallback;
} USARTInfo;

/**
 * @brief External interrupt types
 * @details Use with attachInterrupt()
 */
typedef enum InterruptTrigger {
  RISING, FALLING, CHANGE
} InterruptTrigger;


// Create a structure for user ISR function pointers
typedef struct {
  void (*handler)();
  uint8_t bPwmIn, pinName;
  volatile int risingEdgeMs, risingEdgeSubMs, pulsewidth, period;
} EXTIChannel;


/* Define attribute */
#if defined   ( __CC_ARM   ) /* Keil uVision 4 */
  #define WEAK (__attribute__ ((weak)))
#elif defined ( __ICCARM__ ) /* IAR Ewarm 5.41+ */
  #define WEAK __weak
#elif defined (  __GNUC__  ) /* GCC CS */
  #define WEAK __attribute__ ((weak))
#endif


// Must be defined in variant.cpp
extern PinInfo PIN_MAP[];
extern TimerInfo TIMER_MAP[];
extern TimebaseChannel TIMEBASE_MAP[];
// extern const uint8_t SYSCLK_TIMEBASE;
extern USARTInfo USART_MAP[];
extern EXTIChannel EXTI_MAP[];


#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus

#endif
