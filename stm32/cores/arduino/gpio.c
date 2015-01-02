
#include "gpio.h"
#include "timer.h"
#include "variant.h"
#include "pins.h"

//---------------------------------------------

void digitalWrite(uint8_t name, LogicValue val) {
  if (val == LOW)
    GPIO_ResetBits(PIN_MAP[name].port, 1<<PIN_MAP[name].pin);
  else if (val == HIGH)
    GPIO_SetBits(PIN_MAP[name].port, 1<<PIN_MAP[name].pin);
  else if (val == TOGGLE)
    PIN_MAP[name].port->ODR ^= (1<<PIN_MAP[name].pin);
    // GPIO_ToggleBits(PIN_MAP[name].port, 1<<PIN_MAP[name].pin);
}

// ============================================================================

void ledInit(uint8_t name, DigitalPolarity polarity, LEDOutputMode mode) {
  bitSet(PIN_MAP[name].ioConfig, IOCFGBIT_BOARDLED);

  if (polarity == ACTIVE_LOW)
    bitSet(PIN_MAP[name].ioConfig, IOCFGBIT_ACTIVELOW);
  else
    bitClear(PIN_MAP[name].ioConfig, IOCFGBIT_ACTIVELOW);

  // Configure the pin
  if (mode == LED_PWM)
  {
    bitSet(PIN_MAP[name].ioConfig, IOCFGBIT_PWM);

    if (name == PA4)
      pinRemap(name, 10, TIMER12, 1);
    else if (name == PA9)
      pinRemap(name, 2, TIMER13, 1);
    else if (name == PB14)
      pinRemap(name, 9, TIMER12, 1);
    else if (name == PB15)
      pinRemap(name, 9, TIMER12, 2);
    else if (name == PA14)
      pinRemap(name, 10, TIMER12, 1);
    else if (name == PA15)
      pinRemap(name, 10, TIMER12, 2);

    pinMode(name, PWM);
  }
  else
  {
    bitClear(PIN_MAP[name].ioConfig, IOCFGBIT_PWM);
    pinMode(name, OUTPUT);
  }

  // Turn off to start
  ledWrite(name, 0);
}

void ledWrite(uint8_t name, float val)
{
  if (!bitRead(PIN_MAP[name].ioConfig, IOCFGBIT_BOARDLED))
    return;

  // Reverse if active low
  if (bitRead(PIN_MAP[name].ioConfig, IOCFGBIT_ACTIVELOW))
    val = 1-val;

  if (bitRead(PIN_MAP[name].ioConfig, IOCFGBIT_PWM))
    analogWrite(name, val);
  else
    digitalWrite(name, (LogicValue)val);
}

typedef struct {
  uint8_t green, yellow, red;
} LEDBarGraphConfig;
LEDBarGraphConfig ledbg;

void ledConfigBarGraph(uint8_t green, uint8_t yellow, uint8_t red)
{
  ledbg.green = green;
  ledbg.yellow = yellow;
  ledbg.red = red;
}

void ledBarGraph(float val, float tgreen, float tyellow, float tred)
{
  ledWrite(ledbg.green, (val > tgreen) ? 1 : 0);
  ledWrite(ledbg.yellow, (val > tyellow) ? 1 : 0);
  ledWrite(ledbg.red, (val > tred) ? 1 : 0);
}


