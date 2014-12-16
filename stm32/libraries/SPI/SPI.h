
#ifndef SPI_h
#define SPI_h

#include "Arduino.h"

/** @addtogroup SPI SPI library
 *  @{
 */

#define SPI_MODE0 0x02
#define SPI_MODE1 0x00
#define SPI_MODE2 0x03
#define SPI_MODE3 0x01

#define SPI_CLOCK_DIV2		SPI_BaudRatePrescaler_2
#define SPI_CLOCK_DIV4		SPI_BaudRatePrescaler_4
#define SPI_CLOCK_DIV8		SPI_BaudRatePrescaler_8
#define SPI_CLOCK_DIV16		SPI_BaudRatePrescaler_16
#define SPI_CLOCK_DIV32		SPI_BaudRatePrescaler_32
#define SPI_CLOCK_DIV64		SPI_BaudRatePrescaler_64
#define SPI_CLOCK_DIV128	SPI_BaudRatePrescaler_128
#define SPI_CLOCK_DIV256	SPI_BaudRatePrescaler_256

// cldoc:begin-category(SPI)

class SPIClass {
private:
	static SPI_InitTypeDef SPI_InitStructure;

	static bool SPI_Bit_Order_Set;
	static bool SPI_Data_Mode_Set;
	static bool SPI_Clock_Divider_Set;
	static bool SPI_Enabled;

  static uint8_t SCK, MISO, MOSI;
  static uint8_t afSCK, afMISO, afMOSI;

public:
	// Not in Arduino
	static void setPins(uint8_t SCK, uint8_t afSCK, uint8_t MISO, uint8_t afMISO, uint8_t MOSI, uint8_t afMOSI);

	static void begin();
	// static void begin(PinName);
	static void end();

	static void setBitOrder(uint8_t);
	static void setDataMode(uint8_t);
	static void setClockDivider(uint8_t);

	static uint8_t transfer(uint8_t _data);

	static void attachInterrupt();
	static void detachInterrupt();

	static bool isEnabled(void);
};

extern SPIClass SPI;

/** @} */ // end of addtogroup

#endif
