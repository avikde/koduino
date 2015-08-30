
#ifndef Wire_h
#define Wire_h

#include "Arduino.h"

#define WIRE_BUFFER_LENGTH 32


//FastMode+ -  1 MHz speed (Rise time = 26ns, Fall time = 2ns, Analog Filter OFF)
//FastMode - 400 KHz speed (Rise time = 100ns, Fall time = 10ns, Analog Filter ON)
//Standard - 100 KHz speed (Rise time = 100ns, Fall time = 10ns, Analog Filter ON)

/**
 * STM32F373 takes a funny TIMINGR and manual analog and digital filter setting
 */
enum WireClockSpeed {
#if defined(SERIES_STM32F37x)
  // WIRE_STANDARD = 0x60C3283A, // 100KHz; works
  WIRE_100K = 0x80941F27, // 100KHz; works
  WIRE_200K = 0x80720713, // 200KHz; check?
  WIRE_400K = 0x6043070E, // 400KHz; works
  // WIRE_FAST_MODE_PLUS = 0x2043070E // 933KHz; works
  WIRE_1M = 0x00C71030 // 1MHz; works
#elif defined(SERIES_STM32F30x)
  WIRE_100K = 0x10805E89, // 100KHz standard
  WIRE_400K = 0x00901850, // 400KHz fast mode
  WIRE_1M = 0x00300A1F // 1MHz fast mode+
#else
  WIRE_100K = 100000, // 100KHz; ?
  WIRE_200K = 200000, // 200KHz; ?
  WIRE_400K = 400000, // 400KHz; ?
  WIRE_1M = 1000000 // 1MHz; ?
#endif
};

/**
 * @brief Wire / I2C / SMBus library (call with global object `Wire`)
 * @details 
 */
class TwoWire : public Stream
{
protected:
  I2C_TypeDef *I2Cx;

  uint8_t txBuffer[WIRE_BUFFER_LENGTH];
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

  /**
   * @brief Start the I2C peripheral and configure the `SDA` and `SCL` pins.
   * @details This must be called before any transfer commands
   */
  void begin();
  void begin(uint8_t);
  void begin(int);

  /**
   * @brief Command to begin a write operation
   * @details Queues the slave address in the write buffer
   * 
   * @param address 7-bit slave address
   */
  void beginTransmission(uint8_t address);

  /**
   * @brief Transfer the I2C commands queued in the write buffer
   * @details This command actually does the I2C transfer, i.e. will consume a large 
   * amount of time. It blocks till the transfer is finished.
   * 
   * @param stop (Default: `true`.) Whether or not to send a STOP condition. 
   * *Warning:* the bus will hang unless a STOP is (eventually) sent. 
   *
   * @return One of
   * * 0:success
   * * 1:data too long to fit in transmit buffer
   * * 2:received NACK on transmit of address
   * * 3:received NACK on transmit of data
   * * 4:other error
   */
  uint8_t endTransmission(bool stop);

  /**
   * @overload
   */
  uint8_t endTransmission() { return endTransmission(true); }

  /**
   * @brief Add a single byte to the write buffer
   * @details Queues a byte to be sent
   * 
   * @param b
   * @return `1` if successfull, `0` if write buffer is full
   */
  virtual size_t write(uint8_t b);
  virtual size_t write(const uint8_t *, size_t);

  /**
   * @overload
   */
  uint8_t requestFrom(uint8_t address, uint8_t quantity) {
    return requestFrom(address, quantity, true);
  }

  /**
   * @brief Begin an I2C receive transfer operation
   * @details This function opens the I2C line, sends the slave address and
   * blocks and waits till quantity bytes are received. There is a timeout of ~600us
   * and if nothing is received in that time, the function will return prematurely with
   * the number of bytes received. The received bytes are left in the receive buffer.
   * 
   * @param address 7-bit slave address
   * @param quantity Number of bytes to request
   * @param stop (Default: `true`.) Whether or not to send a STOP condition. 
   * *Warning:* the bus will hang unless a STOP is (eventually) sent. 
   * @return Number of bytes received before a timeout.
   */
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
  using Print::write;

  static void slaveEventHandler(void);
  static bool isEnabled(void);

  // Custom API =================================================

  /**
   * @brief Set SCL clock speed
   * @details Call this before begin()
   * 
   * @param  One of `WIRE_100K`, `WIRE_200K`, `WIRE_400K`, or `WIRE_1M` (with units of Hz)
   */
  void setSpeed(WireClockSpeed);
  // void enableDMAMode(bool);

  /**
   * @brief Enable I2C clock stretching
   * @details [Clock stretching](http://www.i2c-bus.org/clock-stretching/) is an I2C bus
   * feature which may help slow slave devices keep to the timing enforced by the master.
   * This needs to be called before begin()
   * *Warning: untested.*
   * 
   * @param flag `true` to enable, `false` to disable.
   */
  void stretchClock(bool flag);
};

extern TwoWire Wire;
extern TwoWire Wire2;


#endif

