
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

/** @addtogroup Serial Serial / USART
 *  @{
 */

class USARTClass : public Stream {
private:
  // should this be static? i would think one per instance
  static USART_InitTypeDef USART_InitStructure;
  // static bool USARTSerial_Enabled;
  // bool transmitting;
  RingBuffer _rxBuf;
  RingBuffer _txBuf;
  USARTInfo *usartMap; // pointer to USART_MAP[] containing USART peripheral register locations (etc)

  // helper
  void init(uint32_t baud, uint32_t wordLength, uint32_t parity, uint32_t stopBits);

public:
  USARTClass(USARTInfo *usartMapPtr);
  // virtual ~USARTClass() {};
  void begin(uint32_t, uint8_t);
  void begin(uint32_t baud) { begin(baud, SERIAL_8N1); }
  // void beginHalfDuplex(uint32_t);//???
  void end();

  // Custom function to remap pins - call before begin(). Note that USART is always AF7.
  void setPins(uint8_t tx, uint8_t rx);

  virtual int available(void);
  virtual int peek(void);
  virtual int read(void);
  virtual bool writeComplete();
  virtual void flush(void);
  // override a block write from Print
  // virtual size_t write(const uint8_t *buffer, size_t size);
  virtual size_t write(uint8_t c);// { return write(&c, 1); }
  // virtual size_t write(uint8_t);

  // these don't seem right
  // inline size_t write(unsigned long n) { return write((uint8_t)n); }
  // inline size_t write(long n) { return write((uint8_t)n); }
  // inline size_t write(unsigned int n) { return write((uint8_t)n); }
  // inline size_t write(int n) { return write((uint8_t)n); }

  using Print::write; // pull in write(str) from Print

  operator bool() { return true; }

  // Expose RX interrupt for PacketParser
  virtual void attachInterrupt(ByteFunc);
  virtual void detachInterrupt();

// static bool isEnabled(void);
};

/** @} */ // end of addtogroup

extern "C"{
#endif // __cplusplus

void wirishUSARTInterruptHandler(USARTInfo *usartMap);

#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus

#endif
