
#ifndef Wire_h
#define Wire_h

#include "Arduino.h"

#define WIRE_BUFFER_LENGTH 32

// cldoc:begin-category(Wire)

// STM32F373 takes a funny TIMINGR and manual analog and digital filter setting
//FastMode+ -  1 MHz speed (Rise time = 26ns, Fall time = 2ns, Analog Filter OFF)
//FastMode - 400 KHz speed (Rise time = 100ns, Fall time = 10ns, Analog Filter ON)
//Standard - 100 KHz speed (Rise time = 100ns, Fall time = 10ns, Analog Filter ON)
enum WireClockSpeed {
  // WIRE_STANDARD = 0x60C3283A, // 100KHz; works
  WIRE_100K = 0x80941F27, // 100KHz; works
  WIRE_200K = 0x80720713, // 200KHz; check?
  WIRE_400K = 0x6043070E, // 400KHz; works
  // WIRE_FAST_MODE_PLUS = 0x2043070E // 933KHz; works
  WIRE_1M = 0x00C71030 // 1MHz; works
};

class TwoWire// : public Stream
{
protected:
  I2C_TypeDef *I2Cx;

  uint8_t txBuffer[WIRE_BUFFER_LENGTH];
  // uint8_t txBufferIndex;
  uint8_t txBufferLength;
  uint8_t rxBuffer[WIRE_BUFFER_LENGTH];
  uint8_t rxBufferIndex;
  uint8_t rxBufferLength;

  WireClockSpeed clockSpeed;

  uint8_t txAddress;

  bool transmitting;
  bool txBufferFull;
  void (*user_onRequest)(void);
  void (*user_onReceive)(int);

  bool readHelper(uint8_t slaveAddr, uint8_t length, uint8_t *data);

public:

  TwoWire(I2C_TypeDef *I2Cx);
  void begin();
  void begin(uint8_t);
  void begin(int);

  void beginTransmission(uint8_t address);
  uint8_t endTransmission(bool stop);
  uint8_t endTransmission() { return endTransmission(true); }
  virtual size_t write(uint8_t);
  virtual size_t write(const uint8_t *, size_t);

  uint8_t requestFrom(uint8_t address, uint8_t quantity) {
    return requestFrom(address, quantity, true);
  }
  uint8_t requestFrom(uint8_t address, uint8_t quantity, bool stop);
  virtual int available(void);
  virtual int read(void);
  virtual int peek(void);
  virtual void flush(void);

  void onReceive( void (*)(int) );
  void onRequest( void (*)(void) );

  inline size_t write(unsigned long n) { return write((uint8_t)n); }
  inline size_t write(long n) { return write((uint8_t)n); }
  inline size_t write(unsigned int n) { return write((uint8_t)n); }
  inline size_t write(int n) { return write((uint8_t)n); }
  // using Print::write;

  static void slaveEventHandler(void);
  static bool isEnabled(void);

  // Custom API =================================================

  void setSpeed(WireClockSpeed);
  // void enableDMAMode(bool);
  void stretchClock(bool);
};

extern TwoWire Wire;
// extern TwoWire Wire2;

// cldoc:end-category()

#endif

