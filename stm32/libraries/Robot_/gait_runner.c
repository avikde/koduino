
#include "gait_runner.h"
#include "util.h"
#include "system_clock.h"
#include "usart.h"

// Global variables for gaitrunner with some sensible initial values
GaitParams gaitParams = {
  .duty = 0.5,
  .offset = radians(-15),
  .sweep = radians(70),
  // experimental toe-stubbing at the end of slow phase
  .stubSweep = 0, // as angle (probably want this to be -ve). math is done so that the slow phase travel = sweep AFTER this. I.e., if stubSweep < 0, then it will overshoot sweep so as to get back to sweep after the stub
  .stubDuty = 0 // in "phase" units, obviously should be less than duty
};
float gaitPhase = 0.0;
float gaitSpeed = 0.0;
// To adjust the speed
RateMeasurerType gaitRate;

float buehlerClock(const GaitParams *params, float phase)
{
  phase = fmodf_0_1(phase);
  // fraction into phase
  float frac = 0;

  // Stubbing: experimental
  if (phase < params->duty - params->stubDuty)
  {
    // Slow phase - reduces to regular buehler clock if stubSweep stubDuty = 0
    frac = phase / (params->duty - params->stubDuty);
    return params->offset + (params->sweep - params->stubSweep) * frac;
  }
  else if (phase < params->duty)
  {
    // Stub phase (skipped over if stubDuty = 0)
    frac = (phase - (params->duty - params->stubDuty)) / (params->duty - params->stubDuty);
    return params->offset + (params->sweep - params->stubSweep) + frac * params->stubSweep;
  }
    // Slow phase
    // HACK toe stub?
    // if (phase > params->duty * 0.8)
    //   return params->offset;
    // else
  else
  {
    // Fast phase
    frac = (phase - params->duty)/(1.0 - params->duty);
    return params->offset + params->sweep + frac * (TWO_PI - params->sweep);
  }
}


void gaitRunnerUpdate(uint32_t t)
{
  // FIXME: transients on startup? or when gaitrunner is stopped in the middle?
  // rateMeasurerUpdate(&gaitRate);
  // Should result in gaitSpeed being leg freq in Hz
  // gaitPhase += rateMeasurerDt(&gaitRate)/((float)1000000) * gaitSpeed;
  gaitPhase += gaitSpeed / loopRateHz;
}

void gaitRunnerStop()
{
  
}

// =======================================================================

// Compare with the TX script
typedef enum {
  DPAD_UP = 12, DPAD_DOWN = 13, DPAD_LEFT = 14, DPAD_RIGHT = 15 
} DPadButtons;
// Define the menu
typedef enum {
  GAIT_SPEED = 0, GAIT_DUTY, GAIT_OFFSET, GAIT_SWEEP
} GaitRunnerMenuItem;
const uint8_t numMenuItems = 4;
const char * menuItemNames[] = {
  "speed", "duty", "offset", "sweep"
};
const float menuIncrements[] = {1.0, 0.2, 0.2, 0.2};
// Variable to hold state
GaitRunnerMenuItem currentMenuItem = GAIT_SPEED;

void gaitRunnerJoystickMenu(int seriali, uint8_t b)
{
  if (b == DPAD_LEFT || b == DPAD_RIGHT)
  {
    currentMenuItem = (b == DPAD_LEFT) ? (currentMenuItem - 1) : (currentMenuItem + 1);
    currentMenuItem = currentMenuItem % numMenuItems;
    serialPrintf(seriali, "[J] Now changing %s\n", menuItemNames[currentMenuItem]);
  }
  else if (b == DPAD_UP || b == DPAD_DOWN)
  {
    float delta = menuIncrements[currentMenuItem] * ((b==DPAD_UP) ? 1.0 : -1.0);
    switch (currentMenuItem)
    {
      case GAIT_SPEED:
        gaitSpeed += delta;
        serialPrintf(seriali, "[J] %s = %.1f\n", menuItemNames[currentMenuItem], gaitSpeed);
        break;
      case GAIT_DUTY:
        gaitParams.duty += delta;
        serialPrintf(seriali, "[J] %s = %.1f\n", menuItemNames[currentMenuItem], gaitParams.duty);
        break;
      case GAIT_OFFSET:
        gaitParams.offset += delta;
        serialPrintf(seriali, "[J] %s = %.1f deg\n", menuItemNames[currentMenuItem], degrees(gaitParams.offset));
        break;
      case GAIT_SWEEP:
        gaitParams.sweep += delta;
        serialPrintf(seriali, "[J] %s = %.1f deg\n", menuItemNames[currentMenuItem], degrees(gaitParams.sweep));
        break;
      default:
        break;
    }
  }
}
