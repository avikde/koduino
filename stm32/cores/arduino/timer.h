
#ifndef _timer_h_
#define _timer_h_

#include "chip.h"
#include "types.h"

#ifdef __cplusplus
extern "C"{
#endif // __cplusplus


// cldoc:begin-category(Helper)

// Basic user-facing interrupt

// typedef struct 
// {
//   TIM_TypeDef *TIMx;
//   PeriphClockType periphClock;
//   uint32_t RCC_x;
//   IRQn_Type IRQn;
//   uint8_t numChannels;
// } TimerInfo;

// // Indexed by uint8_t
// extern const TimerInfo TIMER_MAP[];

// // Timer configuration - changes at runtime
// typedef struct {
//   // PulseIn
//   volatile int risingEdge, pulseWidth, period;
//   volatile uint32_t lastRollover;
//   uint8_t bPulseIn;
//   uint8_t pinName;
// } TimerChannelData;
// typedef struct {
//   volatile uint32_t numRollovers;
//   // variable number of channels; assigned in timer.c
//   TimerChannelData *channelData;
// } TimerCfg;
// // Indexed by uint8_t
// extern TimerCfg TIMER_MAP[];

// // Helpful to define board pin usage
// typedef struct {
//   uint8_t pin;
//   uint8_t af;
//   uint8_t timer;
//   uint8_t ch;
// } TimerPin;


// cldoc:end-category()

// User functions =========================================================

// 1KHz won't work on 16-bit timers! 72000/1 = 72000 > 0xffff = 65535!
// Use prescaler to scale down to 36MHz base clock? PulseIn is using a 4MHz base clock
// 36MHz base clock => when PWM freq = 20KHz, resolution = 36000/20 = 1800 ~= 11 bits
// Can pulseIn use 36MHz base clock?
// Currently: 4000000/65535 = 61Hz
// 36000000/65535 = 549Hz
// Ideally set 36000/1 => period = 36000, and 1KHz output PWM freq
// For now pulseIn will only be *guaranteed* to work for 1KHz PWM


// cldoc:begin-category(PWM)

void analogWriteFrequency(uint8_t pin, int freqHz);

// Setup PWM on TIMx (tested on TIM3, which has 4 channels)
// * see pin functions on datasheet tables 11-17 to know which pins can 
//   be used with which timer
// * before this command, configure those pins for the right AF using
//   GPIO_PinAFConfig (Section 13.2.7.1 of peripheral guide)
void timerInit(uint8_t timer, int freqHz);

void pinTimerInit(uint8_t pin);

// typedef enum TimerPinMode {
//   PWM, PULSE_IN
// } TimerPinMode;

// Move pulseIn in here
// Should just call timerInit which sets the period (PWM freq), and then separate calls to each channel
// analogWrite / pulseIn should check if the (a) pin is attached to a timer/channel - has pinModeTimer been called?,
// (b) if it is in output (PWM) or input (pulseIn) mode, 
// pinModeTimer should take another argument: ENCODER_AB, PULSE_IN or PWM_OUT
// if pulseIn is called and it is in output mode it should be re-inited TIM_ICInit etc. <- function timerInitPulseInPin
// if analogWrite is called and it is in input mode it should be re-inited TIM_OC1Init etc. <- timerInitPWMPin
// all timer interrupts should be in one .c file
// The struct for pulseIn should be in PIN_MAP: almost any pin could have pulseIn
// New functions:
// void pinModeTimer(uint8_t name, uint8_t af, uint8_t timer, uint8_t channel, TimerPinMode mode);
// void pinModeEncoder(uint8_t p1, uint8_t af1, uint8_t p2, uint8_t af2, uint8_t timer, uint16_t cpr);


// analogWrite:
// * first set up pwm using timerInitPwm(TIM3, 2000, 0); etc
// * duty = duty cycle (in [0.0,1.0])
void analogWrite(uint8_t name, float duty);

// pulseIn
float pulseIn(uint8_t name);

// cldoc:end-category()



// cldoc:begin-category(Helper)

// Timer init with period: mostly a helper function
void timerInitHelper(uint8_t timer, uint16_t prescaler, uint32_t period);

// Main GP timer ISR
void timerISR(uint8_t timer);

// cldoc:end-category()


#ifdef __cplusplus
} // extern "C"

// cldoc:begin-category(PWM)

// static inline void analogWrite(const TimerPin& p, float duty) {
//   analogWrite(p.pin, duty);
// }
// static inline float pulseIn(const TimerPin& p) {
//   return pulseIn(p.pin);
// }
// cldoc:end-category()

#endif // __cplusplus

#endif

