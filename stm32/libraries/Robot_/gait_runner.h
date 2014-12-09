#ifndef _gait_runner_h_
#define _gait_runner_h_

#include <stdint.h>

// New types ================================================
typedef struct {
  // These are in radians
  float duty, offset, sweep, stubSweep, stubDuty;
} GaitParams;

// Global variables for gaitrunner
extern GaitParams gaitParams;
extern float gaitPhase;
extern float gaitSpeed;

// Methods ==================================================

// Phase is between 0 and 1 (mod performed by this function)
float buehlerClock(const GaitParams *params, float phase);

void gaitRunnerUpdate(uint32_t t);
void gaitRunnerStop();

// Gaitrunner joystick menu, 'b' is one of the D-Pad buttons. It prints text messages to the serial port seriali (thinking of an XBee).
void gaitRunnerJoystickMenu(int seriali, uint8_t b);


#endif
