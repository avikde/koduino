
#ifndef _WIRING_CONSTANTS_
#define _WIRING_CONSTANTS_

#include <stdint.h>

#ifdef __cplusplus
extern "C"{
#endif // __cplusplus



// cldoc:begin-category(Pins)

// This is commented
// #define HIGH 0x1
// #define LOW  0x0
// #define TOGGLE 0x2
  
// Types of pin modes
// 
// This is Arduino compatible, except there are some extra options
// typedef enum PinMode {
//   OUTPUT, OUTPUT_OPEN_DRAIN, INPUT, INPUT_ANALOG, INPUT_PULLUP, INPUT_PULLDOWN, ALTERNATE_FUNCTION, PWM_IN
// } PinMode;

// cldoc:end-category()

// typedef enum {
//   LOW = 0, HIGH = 1, TOGGLE = 2
// } LogicValue;

// #define INPUT 0x0
// #define OUTPUT 0x1
// #define INPUT_PULLUP 0x2

// #define true 0x1
// #define false 0x0

#ifndef PI
#define PI 3.1415926535897932384626433832795f
#endif
#define HALF_PI 1.5707963267948966192313216916398f
#define TWO_PI 6.283185307179586476925286766559f
#define DEG_TO_RAD 0.017453292519943295769236907684886f
#define RAD_TO_DEG 57.295779513082320876798154814105f
#define EULER 2.718281828459045235360287471352f

#define SERIAL  0x0
#define DISPLAY 0x1

enum BitOrder {
	LSBFIRST = 0,
	MSBFIRST = 1
};


//      LOW 0
//      HIGH 1
// #define CHANGE 2
// #define FALLING 3
// #define RISING 4

// #define DEFAULT 1
// #define EXTERNAL 0

// undefine stdlib's abs if encountered
#ifdef abs
#undef abs
#endif // abs

#ifndef min
#define min(a,b) ((a)<(b)?(a):(b))
#endif // min

#ifndef max
#define max(a,b) ((a)>(b)?(a):(b))
#endif // max

#define abs(x) ((x)>0?(x):-(x))
#define constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))
#define round(x)     ((x)>=0?(long)((x)+0.5):(long)((x)-0.5))
#define radians(deg) ((deg)*DEG_TO_RAD)
#define degrees(rad) ((rad)*RAD_TO_DEG)
#define sq(x) ((x)*(x))

// #define interrupts() __enable_irq()
// #define noInterrupts() __disable_irq()

#define lowByte(w) ((uint8_t) ((w) & 0xff))
#define highByte(w) ((uint8_t) ((w) >> 8))

#define bitRead(value, bit) (((value) >> (bit)) & 0x01)
#define bitSet(value, bit) ((value) |= (1UL << (bit)))
#define bitClear(value, bit) ((value) &= ~(1UL << (bit)))
#define bitWrite(value, bit, bitvalue) (bitvalue ? bitSet(value, bit) : bitClear(value, bit))

typedef unsigned int word;

#define bit(b) (1UL << (b))

// TODO: to be checked
typedef uint8_t boolean;
typedef uint8_t byte;


#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus

#endif /* _WIRING_CONSTANTS_ */
