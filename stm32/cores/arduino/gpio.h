
#ifndef _gpio_h_
#define _gpio_h_

#include "types.h"
#include "wiring_constants.h"

#ifdef __cplusplus
extern "C"{
#endif // __cplusplus


// // // TODO: 
// typedef enum {
//   // Digital
//   OUTPUT, OUTPUT_OPEN_DRAIN, INPUT, INPUT_PULLUP, INPUT_PULLDOWN, 
//   // Analog
//   INPUT_ANALOG, 
//   // Timer
//   OUTPUT_PWM, INPUT_ENCODER, INPUT_PULSE_IN,
//   // External interrupt (EXTI)
//   INPUT_INTERRUPT,
//   // Other alternate function
//   ALTERNATE_FUNCTION
// } PinMode;

// // digitalWrite, analogWrite can check if the mode is right, otherwise set it


// cldoc:begin-category(Digital)

#define bitRead(value, bit) (((value) >> (bit)) & 0x01)
#define bitSet(value, bit) ((value) |= (1UL << (bit)))
#define bitClear(value, bit) ((value) &= ~(1UL << (bit)))
#define bitWrite(value, bit, bitvalue) (bitvalue ? bitSet(value, bit) : bitClear(value, bit))

// Logic levels (for setting digitalWrite)
typedef enum LogicValue {
  LOW = 0, HIGH = 1, TOGGLE = 2
} LogicValue;

// Set a digital pin to a given logic level
// @name Pin to set
// @val Can be LOW, HIGH, or TOGGLE (new from Arduino)
void digitalWrite(uint8_t name, LogicValue val);

// Read a digital pin
// @name Pin to read
// 
// @return 0 if LOW, 1 if HIGH
static inline uint8_t digitalRead(uint8_t name) {
  return GPIO_ReadInputDataBit(PIN_MAP[name].port, 1<<PIN_MAP[name].pin);
}

// cldoc:end-category()


// cldoc:begin-category(LED)

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

// cldoc:end-category()

#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus

#endif

