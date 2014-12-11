
#include <stdlib.h>
#include <stdio.h>
#include "adc.h"
#include "pins.h"

////////////////////////////////////////////////////////////////////////////////////

// #define ADC1_DR_Address 0x4001244C

// #define ADC_MAX 9 // 0..6, 8, 9
// int numChannelsUsed = 0;

// // Global setup flags, with defaults
// ADCTriggerType adcMode = CONTINUOUS;
uint8_t ADC_SAMPLE_TIME = ADC_SampleTime_13Cycles5;

// void adcSetMode(ADCTriggerType mode)
// {
//   adcMode = mode;
// }
void analogReadSampleTime(uint8_t sampleTime) {
  ADC_SAMPLE_TIME = sampleTime;
}

////////////////////////////////////////////////////////////////////////////////////

// Global to store conversions
// volatile uint16_t dmaAdcBuffer[ADC_MAX];
// uint8_t adcNumChannels = 0;
// MAFilter *fadc;

// void dmaInit(DMA_Channel_TypeDef *dmaYChannelX, uint16_t bufferSize, uint32_t periphalBaseAddr, uint32_t memoryBaseAddr)
// {
//   DMA_InitTypeDef     DMA_InitStructure;
  
//   DMA_DeInit(dmaYChannelX);
//   DMA_InitStructure.DMA_PeripheralBaseAddr = periphalBaseAddr;
//   DMA_InitStructure.DMA_MemoryBaseAddr = memoryBaseAddr;
//   DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
//   DMA_InitStructure.DMA_BufferSize = bufferSize;
//   DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
//   DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
//   DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
//   DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
//   DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
//   DMA_InitStructure.DMA_Priority = DMA_Priority_High;
//   DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
//   DMA_Init(dmaYChannelX, &DMA_InitStructure);
  
//   DMA_Cmd(dmaYChannelX, ENABLE);
// }


// void adcInit(int numChannels, const uint8_t *pins)
// {
//   // For now
//   ADC_TypeDef *ADCx = ADC1;

//   // RCC
//   RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
//   RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

//   // Use DMA
//   dmaInit(DMA1_Channel1, numChannels, ADC1_DR_Address, (uint32_t)dmaAdcBuffer);

//   // ADC clock: must be < 14 MHz (72 / 6 = 12)
//   RCC_ADCCLKConfig(RCC_PCLK2_Div6);
  
//   ADC_DeInit(ADCx);
  
//   ADC_DMACmd(ADCx, ENABLE);
  
//   ADC_InitTypeDef     ADC_InitStructure;
//   ADC_StructInit(&ADC_InitStructure);
  
//   // Configure the ADC1 in continuous mode
//   ADC_InitStructure.ADC_ScanConvMode = ENABLE;
//   ADC_InitStructure.ADC_ContinuousConvMode = (adcMode == CONTINUOUS) ? ENABLE : DISABLE;
//   ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
//   ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
//   ADC_InitStructure.ADC_NbrOfChannel = numChannels;
//   ADC_Init(ADCx, &ADC_InitStructure);
  
//   for (uint8_t i = 0; i < numChannels; ++i)
//   {
//     pinMode(pins[i], INPUT_ANALOG);
//     ADC_RegularChannelConfig(ADCx, PIN_MAP[pins[i]].adcChannel, i+1, adcSampleTime);
//     PIN_MAP[pins[i]].adcConvIndex = i;
//   }
  
//   // Enable ADC1
//   ADC_Cmd(ADCx, ENABLE);
  
//   // Calibration
//   ADC_ResetCalibration(ADCx);
//   while(ADC_GetResetCalibrationStatus(ADCx));
//   ADC_StartCalibration(ADCx);
//   while(ADC_GetCalibrationStatus(ADCx)); 
  
//   // Enable ADC1 - again?
//   ADC_Cmd(ADCx, ENABLE);

//   numChannelsUsed = numChannels;
// }


// uint16_t analogRead(uint8_t name)
// {
//   if (adcMode == SOFTWARE_START)
//     ADC_SoftwareStartConv(ADC1);
  
//   while ((DMA_GetFlagStatus(DMA1_FLAG_TC1)) == RESET) {}
//   DMA_ClearFlag(DMA1_FLAG_TC1);

//   return dmaAdcBuffer[PIN_MAP[name].adcConvIndex];
// }


// void analogReadBatch(int numChannels, const uint8_t *pins, uint16_t *out)
// {
//   // If in software start mode, trigger conversion
//   if (adcMode == SOFTWARE_START)
//     ADC_SoftwareStartConv(ADC1);

//   // Wait till data is ready
//   while ((DMA_GetFlagStatus(DMA1_FLAG_TC1)) == RESET) {}
//   DMA_ClearFlag(DMA1_FLAG_TC1);

//   // // Read from DMA and convert ALL channels
//   // for (int i=0; i<numChannelsUsed; ++i)
//   //   out[i] = dmaAdcBuffer[i];
//   // Read from DMA and convert
//   for (int i=0; i<numChannels; ++i)
//     out[i] = dmaAdcBuffer[ PIN_MAP[pins[i]].adcConvIndex ];
// }

void adcInit() {
  ADC_InitTypeDef  ADC_InitStructure;
  /* PCLK2 is the APB2 clock */
  /* ADCCLK = PCLK2/6 = 72/6 = 12MHz*/
  RCC_ADCCLKConfig(RCC_PCLK2_Div6);

  /* Enable ADC1 clock so that we can talk to it */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
  /* Put everything back to power-on defaults */
  ADC_DeInit(ADC1);

  /* ADC1 Configuration ------------------------------------------------------*/
  /* ADC1 and ADC2 operate independently */
  // ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
  /* Disable the scan conversion so we do one at a time */
  ADC_InitStructure.ADC_ScanConvMode = DISABLE;
  /* Don't do contimuous conversions - do them on demand */
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
  /* Start conversin by software, not an external trigger */
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
  /* Conversions are 12 bit - put them in the lower 12 bits of the result */
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  /* Say how many channels would be used by the sequencer */
  ADC_InitStructure.ADC_NbrOfChannel = 1;

  /* Now do the setup */
  ADC_Init(ADC1, &ADC_InitStructure);
  /* Enable ADC1 */
  ADC_Cmd(ADC1, ENABLE);

  /* Enable ADC1 reset calibaration register */
  ADC_ResetCalibration(ADC1);
  /* Check the end of ADC1 reset calibration register */
  while(ADC_GetResetCalibrationStatus(ADC1));
  /* Start ADC1 calibaration */
  ADC_StartCalibration(ADC1);
  /* Check the end of ADC1 calibration */
  while(ADC_GetCalibrationStatus(ADC1));
}

uint16_t analogRead(uint8_t pin) {
  // Check ioConfig and set?
  // pinMode(pin, INPUT_ANALOG);

  ADC_RegularChannelConfig(ADC1, PIN_MAP[pin].adcChannel, 1, ADC_SAMPLE_TIME);
  // Start the conversion
  ADC_SoftwareStartConv(ADC1);
  // Wait until conversion completion
  while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
  // Get the conversion value
  return ADC_GetConversionValue(ADC1);
}




// #define MAX_ADCS 3
// uint16_t syncReadBuffer[MAX_ADCS];
// uint8_t syncReadPins[MAX_ADCS];
// uint16_t *analogSyncRead(uint8_t count, ...) {
//   va_list ap;
//   // double tot = 0;

//   va_start(ap, count); //Requires the last fixed parameter (to get the address)
//   static uint8_t pins[MAX_ADCS]; // 3 is max
//   for (int j=0; j<count; j++) {
//     syncReadPins[j] = va_arg(ap, uint8_t);
//   }
//   va_end(ap);
//   return tot/count;
// }
// readings = analogSyncRead(3, pin1, pin2, pin3);

