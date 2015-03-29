
#include <stdlib.h>
#include <stdio.h>
#include "adc.h"
#include "pins.h"

////////////////////////////////////////////////////////////////////////////////////


#if defined(SERIES_STM32F37x) || defined(SERIES_STM32F10x)
uint8_t ADC_SAMPLE_TIME = ADC_SampleTime_13Cycles5;
#elif defined(SERIES_STM32F4xx)
uint8_t ADC_SAMPLE_TIME = ADC_SampleTime_15Cycles;
#endif


void analogReadSampleTime(uint8_t sampleTime) {
  ADC_SAMPLE_TIME = sampleTime;
}

void adcCommonInit() {
#if defined(SERIES_STM32F4xx)
  ADC_DeInit();

  ADC_CommonInitTypeDef ADC_CommonInitStructure;
  ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
  ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;
  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
  ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
  ADC_CommonInit(&ADC_CommonInitStructure);
#endif
}

void adcInit(ADC_TypeDef *ADCx) {
  ADC_InitTypeDef  ADC_InitStructure;

#if defined(SERIES_STM32F37x)
  ADC_DeInit(ADCx);
#endif

  ADC_InitStructure.ADC_ScanConvMode = DISABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
#if defined(SERIES_STM32F37x) || defined(SERIES_STM32F10x)
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
  ADC_InitStructure.ADC_NbrOfChannel = 1;
#else
  ADC_InitStructure.ADC_NbrOfConversion = 1;
#endif
  ADC_Init(ADCx, &ADC_InitStructure);
  ADC_Cmd(ADCx, ENABLE);

#if defined(SERIES_STM32F37x) || defined(SERIES_STM32F10x)
  ADC_ResetCalibration(ADCx);
  while(ADC_GetResetCalibrationStatus(ADCx));
  ADC_StartCalibration(ADCx);
  while(ADC_GetCalibrationStatus(ADCx));
#endif
}


uint16_t analogRead(uint8_t pin) {
  // Check ioConfig and set?
  // pinMode(pin, INPUT_ANALOG);

  ADC_RegularChannelConfig(ADC1, PIN_MAP[pin].adcChannel, 1, ADC_SAMPLE_TIME);
  // Start the conversion
#if defined(SERIES_STM32F10x)
  ADC_SoftwareStartConvCmd(ADC1, ENABLE);
#else
  ADC_SoftwareStartConv(ADC1);
#endif

  // Wait until conversion completion
  while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
  // Get the conversion value
  return ADC_GetConversionValue(ADC1);
}



#if defined(SERIES_STM32F4xx)
#define MAX_ADCS 3
uint16_t syncReadBuffer[MAX_ADCS];

const uint16_t *analogSyncRead(uint8_t pin1, uint8_t pin2, uint8_t pin3) {
  ADC_RegularChannelConfig(ADC1, PIN_MAP[pin1].adcChannel, 1, ADC_SAMPLE_TIME);
  ADC_RegularChannelConfig(ADC2, PIN_MAP[pin2].adcChannel, 1, ADC_SAMPLE_TIME);
  ADC_RegularChannelConfig(ADC3, PIN_MAP[pin3].adcChannel, 1, ADC_SAMPLE_TIME);

  ADC_SoftwareStartConv(ADC1);
  ADC_SoftwareStartConv(ADC2);
  ADC_SoftwareStartConv(ADC3);

  while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
  while(ADC_GetFlagStatus(ADC2, ADC_FLAG_EOC) == RESET);
  while(ADC_GetFlagStatus(ADC3, ADC_FLAG_EOC) == RESET);
  // Get the conversion value
  syncReadBuffer[0] = ADC_GetConversionValue(ADC1);
  syncReadBuffer[1] = ADC_GetConversionValue(ADC2);
  syncReadBuffer[2] = ADC_GetConversionValue(ADC3);

  return (const uint16_t *)&syncReadBuffer;
}

const uint16_t *analogSyncRea2(uint8_t pin1, uint8_t pin2) {
  ADC_RegularChannelConfig(ADC1, PIN_MAP[pin1].adcChannel, 1, ADC_SAMPLE_TIME);
  ADC_RegularChannelConfig(ADC2, PIN_MAP[pin2].adcChannel, 1, ADC_SAMPLE_TIME);

  ADC_SoftwareStartConv(ADC1);
  ADC_SoftwareStartConv(ADC2);

  while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
  while(ADC_GetFlagStatus(ADC2, ADC_FLAG_EOC) == RESET);
  // Get the conversion value
  syncReadBuffer[0] = ADC_GetConversionValue(ADC1);
  syncReadBuffer[1] = ADC_GetConversionValue(ADC2);

  return (const uint16_t *)&syncReadBuffer;
}
#endif
