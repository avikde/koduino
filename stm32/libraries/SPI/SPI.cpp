
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

//
SPIClass SPI;

SPI_InitTypeDef SPIClass::SPI_InitStructure;
bool SPIClass::SPI_Bit_Order_Set = false;
bool SPIClass::SPI_Data_Mode_Set = false;
bool SPIClass::SPI_Clock_Divider_Set = false;
bool SPIClass::SPI_Enabled = false;

// // Controller_v1
// PinName SPIClass::SS = PA11;
// PinName SPIClass::SCK = PA12;
// PinName SPIClass::MISO = PA13;
// PinName SPIClass::MOSI = PF6;

// M4
// PinName SPIClass::SS = PB3;
uint8_t SPIClass::SCK = PA5;
uint8_t SPIClass::MISO = PB4;
uint8_t SPIClass::MOSI = PB5;
uint8_t SPIClass::afSCK = 5;
uint8_t SPIClass::afMISO = 5;
uint8_t SPIClass::afMOSI = 5;

void SPIClass::setPins(uint8_t SCK, uint8_t afSCK, uint8_t MISO, uint8_t afMISO, uint8_t MOSI, uint8_t afMOSI) {
	SPIClass::SCK = SCK;
	SPIClass::MISO = MISO;
	SPIClass::MOSI = MOSI;
	SPIClass::afSCK = afSCK;
	SPIClass::afMISO = afMISO;
	SPIClass::afMOSI = afMOSI;
}

void SPIClass::begin() {
	// if (ss_pin >= TOTAL_PINS )
	// {
	// 	return;
	// }
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);

	pinModeAlt(SCK, GPIO_OType_PP, GPIO_PuPd_DOWN, afSCK);
	pinModeAlt(MISO, GPIO_OType_PP, GPIO_PuPd_DOWN, afMISO);
	pinModeAlt(MOSI, GPIO_OType_PP, GPIO_PuPd_DOWN, afMOSI);

	SPI_I2S_DeInit(SPI1);

	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	if(SPI_Data_Mode_Set != true)
	{
		//Default: SPI_MODE0
		SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
		SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	}
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	if(SPI_Clock_Divider_Set != true)
	{
		SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
	}
	if(SPI_Bit_Order_Set != true)
	{
		//Default: MSBFIRST
		SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	}
	SPI_InitStructure.SPI_CRCPolynomial = 7;

	SPI_Init(SPI1, &SPI_InitStructure);

	// Important
  SPI_SSOutputCmd(SPI1, ENABLE);
#if defined(STM32F37x)
  SPI_RxFIFOThresholdConfig(SPI1, SPI_RxFIFOThreshold_QF);
#endif

	SPI_Cmd(SPI1, ENABLE);

	SPI_Enabled = true;
}

void SPIClass::end() {
	if(SPI_Enabled != false)
	{
		SPI_Cmd(SPI1, DISABLE);

		SPI_Enabled = false;
	}
}

void SPIClass::setBitOrder(uint8_t bitOrder)
{
	if(bitOrder == LSBFIRST)
	{
		SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_LSB;
	}
	else
	{
		SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	}

	SPI_Init(SPI1, &SPI_InitStructure);

	SPI_Bit_Order_Set = true;
}

void SPIClass::setDataMode(uint8_t mode)
{
	if(SPI_Enabled != false)
	{
		SPI_Cmd(SPI1, DISABLE);
	}

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

	SPI_Init(SPI1, &SPI_InitStructure);

	if(SPI_Enabled != false)
	{
		SPI_Cmd(SPI1, ENABLE);
	}

	SPI_Data_Mode_Set = true;
}

void SPIClass::setClockDivider(uint8_t rate)
{
	SPI_InitStructure.SPI_BaudRatePrescaler = rate;

	SPI_Init(SPI1, &SPI_InitStructure);

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
  while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE))
    spiRX(SPI1);

  //wait until TX buffer is empty
  while(!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE));

  spiTX(SPI1, cmd);  //send the command
  while(!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE));
  return spiRX(SPI1);
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
