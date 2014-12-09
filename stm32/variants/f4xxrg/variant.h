#ifndef variant_h
#define variant_h

#include "chip.h"

#ifdef __cplusplus
extern "C"{
#endif // __cplusplus

// PINS
extern const uint8_t PA0;
extern const uint8_t PA1;
extern const uint8_t PA2;
extern const uint8_t PA3;
extern const uint8_t PA4;
extern const uint8_t PA5;
extern const uint8_t PA6;
extern const uint8_t PA8;
extern const uint8_t PA9;
extern const uint8_t PA10;
extern const uint8_t PA11;
extern const uint8_t PA12;
extern const uint8_t PA13;
extern const uint8_t PA14;
extern const uint8_t PA15;
extern const uint8_t PB0;
extern const uint8_t PB1;
extern const uint8_t PB2;
extern const uint8_t PB3;
extern const uint8_t PB4;
extern const uint8_t PB5;
extern const uint8_t PB6;
extern const uint8_t PB7;
extern const uint8_t PB8;
extern const uint8_t PB9;
extern const uint8_t PB14;
extern const uint8_t PB15;
extern const uint8_t PC13;
extern const uint8_t PC14;
extern const uint8_t PC15;
extern const uint8_t PD8;
extern const uint8_t PE8;
extern const uint8_t PE9;
extern const uint8_t PF1;
extern const uint8_t PF6;
extern const uint8_t PF7;

// TIMERS
extern const uint8_t TIMER2;
extern const uint8_t TIMER3;
extern const uint8_t TIMER4;
extern const uint8_t TIMER5;
extern const uint8_t TIMER6;
extern const uint8_t TIMER7;
extern const uint8_t TIMER12;
extern const uint8_t TIMER13;
extern const uint8_t TIMER14;
extern const uint8_t TIMER15;
extern const uint8_t TIMER16;
extern const uint8_t TIMER17;
extern const uint8_t TIMER18;
extern const uint8_t TIMER19;


// Mandated by the core library

#define LED_BUILTIN PC13

// cldoc:end-category()

#ifdef __cplusplus
} // extern "C"

#include "USARTClass.h"

extern USARTClass Serial1; // default PA9, PA10
extern USARTClass Serial2; // default PB3, PB4
extern USARTClass Serial3; // default PB8, PB9


bool isTimer32Bit(uint8_t tim);
uint8_t numChannelsInTimer(uint8_t tim);
bool isAnalogPin(uint8_t pin);
void variantInit();

#endif // __cplusplus

#endif

