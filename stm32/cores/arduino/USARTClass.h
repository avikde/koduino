/**
 * @authors Particle Industries, Avik De <avikde@gmail.com>

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
#ifndef USARTClass_h
#define USARTClass_h

#include "chip.h"
#include "types.h"
  
// Define config for Serial.begin(baud, config);
// #define SERIAL_5N1 0x00
// #define SERIAL_6N1 0x02
// #define SERIAL_7N1 0x04
#define SERIAL_8N1 0x06
// #define SERIAL_5N2 0x08
// #define SERIAL_6N2 0x0A
// #define SERIAL_7N2 0x0C
#define SERIAL_8N2 0x0E
// #define SERIAL_5E1 0x20
// #define SERIAL_6E1 0x22
#define SERIAL_7E1 0x24
#define SERIAL_8E1 0x26
// #define SERIAL_5E2 0x28
// #define SERIAL_6E2 0x2A
#define SERIAL_7E2 0x2C
#define SERIAL_8E2 0x2E
// #define SERIAL_5O1 0x30
// #define SERIAL_6O1 0x32
#define SERIAL_7O1 0x34
#define SERIAL_8O1 0x36
// #define SERIAL_5O2 0x38
// #define SERIAL_6O2 0x3A
#define SERIAL_7O2 0x3C
#define SERIAL_8O2 0x3E

#ifdef __cplusplus

#include "Stream.h"



/**
 * @brief Serial / USART library (use on global objects `Serial<x>` where `<x>` can be 1, 2, 3)
 * @details 
 */
class USARTClass : public Stream {
public:
  // should this be static? i would think one per instance
  static USART_InitTypeDef USART_InitStructure;
  // static bool USARTSerial_Enabled;
  // bool transmitting;
  RingBuffer _rxBuf;
  RingBuffer _txBuf;
  USARTInfo *usartMap; // pointer to USART_MAP[] containing USART peripheral register locations (etc)

  // helper
  void init(uint32_t baud, uint32_t wordLength, uint32_t parity, uint32_t stopBits);

// public:
  uint8_t irqnPriority;
  
  USARTClass(USARTInfo *usartMapPtr);
  // virtual ~USARTClass() {};

  /**
   * @brief Opens a serial connection and configures the RX and TX pins
   * @details Use setPins() to change the default RX and TX pins
   * 
   * @param baud Baud rate as an integer
   * @param mode One of `SERIAL_<x>` where `<x>` can be `8N1`, `8N2`, `7E1`, `8E1`, 
   * `7E2`, `8E2`, `7O1`, `8O2` (default if omitted is `8N1`)
   */
  void begin(uint32_t baud, uint8_t mode);

  void begin(uint32_t baud) { begin(baud, SERIAL_8N1); }

  /**
   * @brief Close the serial port
   * @details 
   */
  void end();

  /**
   * @brief Change the default RX and TX pins
   * @details Should be called before begin(). Defaults are
   * 
   * * `PA9`, `PA10` for `Serial1`
   * * `PB3`, `PB4` for `Serial2`
   * * `PB8`, `PB9` for `Serial3`
   * 
   * @param tx New TX pin
   * @param rx New RX pin
   */
  void setPins(uint8_t tx, uint8_t rx);

  virtual int available(void);
  virtual int peek(void);
  virtual int read(void);
  virtual void flush(void);

  void stopTX();

  /**
   * @brief Check if the previous write operation is finished
   * @details 
   * @return `true` if complete
   */
  virtual bool writeComplete();

  // Custom functions

  // peekAt(0) should return the same as peek()
  uint8_t peekAt(uint8_t pos);
  void flushInput();

  // override a block write from Print
  // virtual size_t write(const uint8_t *buffer, size_t size);

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

  // // Expose RX interrupt for PacketParser
  // virtual void attachInterrupt(ByteFunc);
  // virtual void detachInterrupt();

  // new for RS485
  void attachBus(void *busObject) {usartMap->busObject = busObject;}

// static bool isEnabled(void);
};

// CUSTOM INTERRUPT HANDLER

extern "C"{
#endif // __cplusplus

// NEW for bus. these are defined in the bus class
void busHandlerTC(void *busObject) WEAK;
void busHandlerRX(void *busObject, uint8_t byte) WEAK;
// Regular USART handlers

static inline void ringBufferStore(unsigned char c, RingBuffer *buffer) __attribute__((always_inline, unused));
static inline void ringBufferStore(unsigned char c, RingBuffer *buffer) {
  uint8_t next = (uint8_t)(buffer->head + 1) % SERIAL_BUFFER_SIZE;

  if (next != buffer->tail) {
    buffer->head = next;
    // head==tail means empty, should store at old_head+1
    buffer->buffer[next] = c;
  }
}


static inline void wirishUSARTInterruptHandler(USARTInfo *usartMap) __attribute__((always_inline, unused));
static inline void wirishUSARTInterruptHandler(USARTInfo *usartMap)
{
  if(USART_GetITStatus(usartMap->USARTx, USART_IT_RXNE) != RESET) {
    // Read byte from the receive data register
    uint8_t c = USART_ReceiveData(usartMap->USARTx);
    // If interrupt is attached then don't use the RingBuffer
    if (usartMap->busObject != 0)
      busHandlerRX(usartMap->busObject, c);
    else
      ringBufferStore(c, usartMap->rxBuf);
  }
  if(USART_GetITStatus(usartMap->USARTx, USART_IT_TXE) != RESET) {
    // Write byte to the transmit data register
    if (usartMap->txBuf->head == usartMap->txBuf->tail) {
      // Buffer empty, so disable the USART Transmit interrupt
      USART_ITConfig(usartMap->USARTx, USART_IT_TXE, DISABLE);
      // if (usartMap->busObject != 0) {
      //   // delayMicroseconds(1);
      //   busHandlerTC(usartMap->busObject);
      // }
    }
    else {
      // There is more data in the output buffer. Send the next byte
      USART_SendData(usartMap->USARTx, usartMap->txBuf->buffer[usartMap->txBuf->tail++]);
      usartMap->txBuf->tail %= SERIAL_BUFFER_SIZE;
    }
  }
  // New for bus: IT_TC is only enabled if a busObject is set
  if(USART_GetITStatus(usartMap->USARTx, USART_IT_TC) != RESET) {
    USART_ClearITPendingBit(usartMap->USARTx, USART_IT_TC);
    USART_ITConfig(usartMap->USARTx, USART_IT_TC, DISABLE);
    // this will link only if bus class is included
    busHandlerTC(usartMap->busObject);
  }
}

#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus

#endif
