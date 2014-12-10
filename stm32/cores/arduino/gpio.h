
#ifndef _gpio_h_
#define _gpio_h_

#include "types.h"
#include "wiring_constants.h"

#ifdef __cplusplus
extern "C"{
#endif // __cplusplus



/** @addtogroup Digital Digital I/O
 *  @{
 */

#define bitRead(value, bit) (((value) >> (bit)) & 0x01)
#define bitSet(value, bit) ((value) |= (1UL << (bit)))
#define bitClear(value, bit) ((value) &= ~(1UL << (bit)))
#define bitWrite(value, bit, bitvalue) (bitvalue ? bitSet(value, bit) : bitClear(value, bit))

/**
 * @brief Set a digital pin to a given logic level
 * @details [long description]
 * 
 * @param pin Pin to set
 * @param val Can be `LOW`, `HIGH`, or `TOGGLE` (new from Arduino)
 */
void digitalWrite(uint8_t pin, LogicValue val);

/**
 * @brief Read a digital pin
 * @details Pin must have been configured as input by pinMode()
 * 
 * @param pin Pin to read 
 * @return 1 if the pin is a logic HIGH level, 0 for LOW
 */
static inline uint8_t digitalRead(uint8_t pin) {
  return GPIO_ReadInputDataBit(PIN_MAP[name].port, 1<<PIN_MAP[name].pin);
}

/** @} */

typedef enum DigitalPolarity {
  ACTIVE_HIGH, ACTIVE_LOW
} DigitalPolarity;

typedef enum LEDOutputMode {
  LED_DIGITAL, LED_PWM
} LEDOutputMode;

void ledInit(uint8_t name, DigitalPolarity polarity, LEDOutputMode mode);
// If it was inited as digital, then cast to 0 or 1. If it was inited as PWM, then this is interpreted as a "brightness" value in [0,1]
void ledWrite(uint8_t name, float val);

// Use Red,Yellow,Green LEDs as a bargraph with three thresholds
void ledConfigBarGraph(uint8_t green, uint8_t yellow, uint8_t red);
void ledBarGraph(float val, float tgreen, float tyellow, float tred);


#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus

#endif

