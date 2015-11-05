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

#if defined(STM32F37x)
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
#endif
    
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
    analogWriteFloat(name, val);
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


