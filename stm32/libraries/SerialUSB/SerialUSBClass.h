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
#ifndef SerialUSBClass_h
#define SerialUSBClass_h

#include "chip.h"
#include "types.h"

#ifdef __cplusplus

#include "Stream.h"

class SerialUSBClass : public Stream {
public:
  void begin(uint32_t baud, uint8_t disconnectPin);
  void begin(uint32_t baud) {begin(baud,0xff);}

  /**
   * @brief Close the serial port
   * @details 
   */
  void end();

  virtual int available(void);
  virtual int peek(void);
  virtual int read(void);
  virtual void flush(void);

  // override a block write from Print
  virtual size_t write(const uint8_t *buffer, size_t size);

  /**
   * @brief Write a single character
   * @details Does not block. Uses a ring buffer to queue outgoing transfers and 
   * interrupt handlers to transmit the queue.
   * 
   * *Note:* No output is allowed for the first 1 second on the bootloading port, 
   * `Serial1` in order to not interfere with auto-reset and bootloading
   * 
   * @param c Character to write
   * @return 1 on success, 0 on failure
   */
  virtual size_t write(uint8_t c);// { return write(&c, 1); }
  // virtual size_t write(uint8_t);

  // these don't seem right
  // inline size_t write(unsigned long n) { return write((uint8_t)n); }
  // inline size_t write(long n) { return write((uint8_t)n); }
  // inline size_t write(unsigned int n) { return write((uint8_t)n); }
  // inline size_t write(int n) { return write((uint8_t)n); }

  using Print::write; // pull in write(str) from Print

  operator bool() { return true; }
};

extern SerialUSBClass Serial;

#endif // __cplusplus

#endif
