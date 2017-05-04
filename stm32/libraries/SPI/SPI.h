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
#ifndef SPI_h
#define SPI_h

#include "Arduino.h"

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

// Account for SPL discrepancy: make F4 functions look like F3 functions
#if defined(SERIES_STM32F4xx)
#define SPI_ReceiveData8        SPI_I2S_ReceiveData
#define SPI_I2S_ReceiveData16   SPI_I2S_ReceiveData
#define SPI_SendData8           SPI_I2S_SendData
#define SPI_I2S_SendData16      SPI_I2S_SendData
#endif

/**
 * @brief SPI library (call with global object `SPI`)
 * @details 
 */
class SPIClass {
protected:
	SPI_InitTypeDef SPI_InitStructure;

	bool SPI_Bit_Order_Set;
	bool SPI_Data_Mode_Set;
	bool SPI_Clock_Divider_Set;
	bool SPI_Enabled;

  uint8_t SCK, MISO, MOSI;
  uint8_t afSCK, afMISO, afMOSI;

  uint16_t dataSize;

  uint16_t spiRX();
  void spiTX(uint16_t cmd);

#if defined(SERIES_STM32F37x) || defined(SERIES_STM32F30x)
  // DMA
  DMA_Channel_TypeDef *DMA_Channel_Tx, *DMA_Channel_Rx;
  uint32_t DMA_FLAG_Tx_TC, DMA_FLAG_Rx_TC;
#endif

public:
  SPI_TypeDef *SPIx;
	SPIClass(SPI_TypeDef *SPIx);

	/**
	 * @brief Change default SCK, MISO and MOSI pins
	 * @details You will need to look at the datasheet to fill in the alternate function (AF) numbers
	 * 
	 * @param SCK Default is `PA5`
	 * @param afSCK 
	 * @param MISO Default is `PB4`
	 * @param afMISO 
	 * @param MOSI Default is `PB5`
	 * @param afMOSI 
	 */
	void setPins(uint8_t SCK, uint8_t afSCK, uint8_t MISO, uint8_t afMISO, uint8_t MOSI, uint8_t afMOSI);

	/**
	 * @brief Start SPI peripheral and configure pins
	 * @details 
	 */
	void begin();

	/**
	 * @brief Close SPI
	 * @details 
	 */
	void end();

  /**
   * @brief Sets data size
   * @details default is SPI_DataSize_8b
   * 
   * @param one of SPI_DataSize_Xb where X is from 4..16
   */
  void setDataSize(uint16_t sz) {dataSize = sz;}

	/**
	 * @brief Sets the bit-order for data
	 * @details 
	 * 
	 * @param ord One of `MSBFIRST`, `LSBFIRST`
	 */
	void setBitOrder(uint8_t ord);

	/**
	 * @brief Sets the SPI mode
	 * @details See the [Wikipedia SPI page](http://en.wikipedia.org/wiki/Serial_Peripheral_Interface_Bus) for details on these modes
	 * 
	 * @param mode One of `SPI_MODE<x>`, where `<x>` can be 0, 1, 2, 3
	 */
	void setDataMode(uint8_t mode);

	/**
	 * @brief Sets the clock divider (with respect to the system clock)
	 * @details 
	 * 
	 * @param div One of `SPI_CLOCK_DIV<x>` where `<x>` can be 2, 4, 8, 16, 32, 64, 128, 256
	 */
	void setClockDivider(uint8_t div);

	/**
	 * @brief Transfers one byte over the SPI bus, both sending and receiving
	 * @details Make sure you drive chip select low before calling this
	 * 
	 * @param cmd Byte to send
	 * @return Returns the byte received from the slave
	 */
	inline uint8_t transfer(uint8_t cmd) {
    // discard any remaining bytes in the RX register
    while(SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_RXNE)) {
      #if defined(SERIES_STM32F37x) || defined(SERIES_STM32F30x)
      SPI_ReceiveData8(SPIx);
      #else
      SPI_I2S_ReceiveData(SPIx);
      #endif
    }

    //wait until TX buffer is empty -- doesn't seem like any need to
    // while(!SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_TXE));

    #if defined(SERIES_STM32F37x) || defined(SERIES_STM32F30x)
    SPI_SendData8(SPIx, cmd);  //send the command
    #else
    SPI_I2S_SendData(SPIx, cmd);
    #endif
    // wait till data in
    while(!SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_RXNE));
    #if defined(SERIES_STM32F37x) || defined(SERIES_STM32F30x)
    return SPI_ReceiveData8(SPIx);
    #else
    return SPI_I2S_ReceiveData(SPIx);
    #endif
  }

  // DMA functions ---------------------

#if defined(SERIES_STM32F37x) || defined(SERIES_STM32F30x)
  void initDMA(uint32_t RCC_AHBPeriph, DMA_Channel_TypeDef *DMA_Channel_Tx, DMA_Channel_TypeDef *DMA_Channel_Rx, uint32_t DMA_FLAG_Tx_TC, uint32_t DMA_FLAG_Rx_TC);

  void writeDMA(uint16_t nbytes, const uint8_t *ibuf);

  void readDMA(uint16_t nbytes, uint8_t *obuf, const uint8_t *ibuf=NULL, bool polling=true);
#endif

  // -----------------

	void attachInterrupt();
	void detachInterrupt();

	bool isEnabled(void);
};

extern SPIClass SPI;
extern SPIClass SPI_2;
#if defined(STM32F446xx)
extern SPIClass SPI_3;
#endif

#endif
