
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


SPIClass::SPIClass(SPI_TypeDef *SPIx) {
	// Initialize member variables
	this->SPIx = SPIx;
	SPI_Bit_Order_Set = false;
	SPI_Data_Mode_Set = false;
	SPI_Clock_Divider_Set = false;
	SPI_Enabled = false;

#if defined(SERIES_STM32F37x)
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
		MISO = PA6;
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
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
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

	// Important, but not sure what it does
  SPI_SSOutputCmd(SPIx, ENABLE);
#if defined(STM32F37x)
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
#if defined(STM32F37x)
uint8_t spiRX(SPI_TypeDef *SPIx) {
	return SPI_ReceiveData8(SPIx);
}
void spiTX(SPI_TypeDef *SPIx, uint8_t cmd) {
	SPI_SendData8(SPIx, cmd);
}
#else
uint8_t spiRX(SPI_TypeDef *SPIx) {
	return (uint8_t)SPI_I2S_ReceiveData(SPIx);
}
void spiTX(SPI_TypeDef *SPIx, uint8_t cmd) {
	SPI_I2S_SendData(SPIx, cmd);
}
#endif

uint8_t SPIClass::transfer(uint8_t cmd) {
  //read off any remaining bytes
  while(SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_RXNE))
    spiRX(SPIx);

  //wait until TX buffer is empty
  while(!SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_TXE));

  spiTX(SPIx, cmd);  //send the command
  while(!SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_RXNE));
  return spiRX(SPIx);
}

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
SPIClass SPI(SPI1);
SPIClass SPI_2(SPI2);

