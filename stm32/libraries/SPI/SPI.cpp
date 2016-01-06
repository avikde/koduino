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
#include "SPI.h"
#include "nvic.h"

// typedef struct {
// 	PeriphClockType periphClock;
// 	uint32_t RCC_x;
// 	SPI_TypeDef *SPIx;
// } SPIInfo;
// SPIInfo _SPI[] = {
// 	{
// 		.periphClock = APB2,
// 		.RCC_x = 
// 	}
// }


SPIClass::SPIClass(SPI_TypeDef *SPIx) : SPIx(SPIx), SPI_Bit_Order_Set(false), SPI_Data_Mode_Set(false), SPI_Clock_Divider_Set(false), SPI_Enabled(false), dataSize(SPI_DataSize_8b) {
#if defined(SERIES_STM32F30x)
	// Initialize default pin config
	if (SPIx == SPI2) {
		// Mainboard default (MPU6000 on that board)
		SCK = PB13;
		afSCK = 5;
		MISO = PB14;
		afMISO = 5;
		MOSI = PB15;
		afMOSI = 5;
	}
	else if (SPIx == SPI3) {
		// Mainboard default (SPI header)
		SCK = PC10;
		afSCK = 6;
		MISO = PC11;
		afMISO = 6;
		MOSI = PC12;
		afMOSI = 6;
	}
#elif defined(SERIES_STM32F37x)
	// Initialize default pin config
	if (SPIx == SPI1) {
		// Mainboard default (MPU6000 on that board)
		SCK = PA12;
		afSCK = 6;
		MISO = PA13;
		afMISO = 6;
		MOSI = PF6;
		afMOSI = 5;
	}
	else if (SPIx == SPI2) {
		// Mainboard default
		SCK = PA8;
		afSCK = 5;
		MISO = PB14;
		afMISO = 5;
		MOSI = PB15;
		afMOSI = 5;
	}
	else if (SPIx == SPI3) {
		// TODO
	}
#elif defined(SERIES_STM32F4xx)
	// Initialize default pin config
	if (SPIx == SPI1) {
		// Steensy default
		SCK = PA5;
		afSCK = 5;
		MISO = PB4;
		afMISO = 5;
		MOSI = PA7;
		afMOSI = 5;
	}
	else if (SPIx == SPI2) {
		// FIXME: same as F3 for now
		SCK = PA8;
		afSCK = 5;
		MISO = PB14;
		afMISO = 5;
		MOSI = PB15;
		afMOSI = 5;
	}
	else if (SPIx == SPI3) {
		// TODO
	}
#endif
}


void SPIClass::setPins(uint8_t SCK, uint8_t afSCK, uint8_t MISO, uint8_t afMISO, uint8_t MOSI, uint8_t afMOSI) {
	this->SCK = SCK;
	this->MISO = MISO;
	this->MOSI = MOSI;
	this->afSCK = afSCK;
	this->afMISO = afMISO;
	this->afMOSI = afMOSI;
}

void SPIClass::begin() {
	// Start clock
	if (SPIx == SPI1)
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
	else if (SPIx == SPI2)
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
	else if (SPIx == SPI3)
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);

	pinModeAlt(SCK, GPIO_OType_PP, GPIO_PuPd_DOWN, afSCK);
	pinModeAlt(MISO, GPIO_OType_PP, GPIO_PuPd_DOWN, afMISO);
	pinModeAlt(MOSI, GPIO_OType_PP, GPIO_PuPd_DOWN, afMOSI);

	SPI_I2S_DeInit(SPIx);

	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = dataSize;
	if(SPI_Data_Mode_Set != true) {
		//Default: SPI_MODE0
		SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
		SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	}
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	if(SPI_Clock_Divider_Set != true) {
		SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
	}
	if(SPI_Bit_Order_Set != true) {
		//Default: MSBFIRST
		SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	}
	SPI_InitStructure.SPI_CRCPolynomial = 7;

	SPI_Init(SPIx, &SPI_InitStructure);

	// FIXME: the state of these settings doesn't seem to make sense...but it works for now.
  SPI_SSOutputCmd(SPIx, ENABLE);

#if defined(SERIES_STM32F30x) || defined(SERIES_STM32F37x)
  // Info here: http://stackoverflow.com/questions/22769920/stm32f0-spi-loopback-no-data-on-miso
  if (dataSize == SPI_DataSize_16b)
    SPI_RxFIFOThresholdConfig(SPIx, SPI_RxFIFOThreshold_HF);
  else
    SPI_RxFIFOThresholdConfig(SPIx, SPI_RxFIFOThreshold_QF);
#endif

	SPI_Cmd(SPIx, ENABLE);

	SPI_Enabled = true;
}

void SPIClass::end() {
	if(SPI_Enabled != false) {
		SPI_Cmd(SPIx, DISABLE);
		SPI_Enabled = false;
	}
}

void SPIClass::setBitOrder(uint8_t bitOrder) {
	SPI_InitStructure.SPI_FirstBit = (bitOrder == LSBFIRST) ? SPI_FirstBit_LSB : SPI_FirstBit_MSB;
	SPI_Init(SPIx, &SPI_InitStructure);
	SPI_Bit_Order_Set = true;
}

void SPIClass::setDataMode(uint8_t mode) {
	if(SPI_Enabled != false)
		SPI_Cmd(SPIx, DISABLE);

	switch(mode)
	{
	case SPI_MODE0:
		SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
		SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
		break;

	case SPI_MODE1:
		SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
		SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
		break;

	case SPI_MODE2:
		SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
		SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
		break;

	case SPI_MODE3:
		SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
		SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
		break;
	}

	SPI_Init(SPIx, &SPI_InitStructure);

	if(SPI_Enabled != false)
		SPI_Cmd(SPIx, ENABLE);

	SPI_Data_Mode_Set = true;
}

void SPIClass::setClockDivider(uint8_t rate)
{
	SPI_InitStructure.SPI_BaudRatePrescaler = rate;

	SPI_Init(SPIx, &SPI_InitStructure);

	SPI_Clock_Divider_Set = true;
}

// Helper functions
#if defined(SERIES_STM32F37x) || defined(SERIES_STM32F30x)
uint16_t SPIClass::spiRX() {
  if (dataSize == SPI_DataSize_16b)
    return SPI_I2S_ReceiveData16(SPIx);
  else
    return SPI_ReceiveData8(SPIx);
}
void SPIClass::spiTX(uint16_t cmd) {
	if (dataSize == SPI_DataSize_16b)
    SPI_I2S_SendData16(SPIx, cmd);
  else
    SPI_SendData8(SPIx, (uint8_t)cmd);
}
#else
uint16_t SPIClass::spiRX() {
	return SPI_I2S_ReceiveData(SPIx);
}
void SPIClass::spiTX(uint16_t cmd) {
	SPI_I2S_SendData(SPIx, cmd);
}
#endif

uint8_t SPIClass::transfer(uint8_t cmd) {
  //read off any remaining bytes
  while(SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_RXNE))
    spiRX();

  //wait until TX buffer is empty
  while(!SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_TXE));

  spiTX(cmd);  //send the command
  while(!SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_RXNE));
  return (uint8_t)spiRX();
}

// uint16_t SPIClass::transfer16(uint16_t cmd) {
//   //read off any remaining bytes
//   while(SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_RXNE))
//     spiRX();

//   //wait until TX buffer is empty
//   while(!SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_TXE));

//   spiTX(cmd);  //send the command
//   while(!SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_RXNE));
//   return spiRX();
// }

void SPIClass::attachInterrupt() {
	//To Do
}

void SPIClass::detachInterrupt() {
	//To Do
}

bool SPIClass::isEnabled() {
	return SPI_Enabled;
}



#ifdef __cplusplus
extern "C"{
#endif // __cplusplus

void Wiring_SPI1_Interrupt_Handler(void)
{
	//To Do
}

#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus



// Initialize 
#if defined(SERIES_STM32F30x)
SPIClass SPI(SPI3);
#else
SPIClass SPI(SPI1);
#endif
SPIClass SPI_2(SPI2);

