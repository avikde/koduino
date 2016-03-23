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
#include "SerialUSBClass.h"
#include "mUSB.h"

void SerialUSBClass::begin(uint32_t baud, uint8_t disconnectPin) {
  setUSBDisconnectPin(disconnectPin);
  setUSBBaudRate(baud);
  mUSBInit();
}

void SerialUSBClass::end() {
}

int SerialUSBClass::available(void) {
  return 0;
}

int SerialUSBClass::peek(void) {
  return 0;
}

int SerialUSBClass::read(void) {
  volatile char ptr;
  usbRead(&ptr, 1);
  return ptr;
}

size_t SerialUSBClass::write(uint8_t c) {
  return usbWrite((char *)&c, 1);
}

size_t SerialUSBClass::write(const uint8_t *buffer, size_t size) {
  return usbWrite((char *)buffer, size);
}


void SerialUSBClass::flush() {

}

SerialUSBClass Serial;
