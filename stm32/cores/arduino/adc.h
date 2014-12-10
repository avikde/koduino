
#ifndef _adc_h_
#define _adc_h_

#include "chip.h"
#include "types.h"

#ifdef __cplusplus
extern "C"{
#endif // __cplusplus

// cldoc:begin-category(Helper)
  
// Init DMA for ADC
// 
// numChannels must be smaller than or equal to 9 (ADC1 has 9 channels on the chip)
// void dmaInit(DMA_Channel_TypeDef *dmaYChannelX, uint16_t bufferSize, uint32_t periphalBaseAddr, uint32_t memoryBaseAddr);

// cldoc:end-category()

// cldoc:begin-category(Analog)

// typedef enum ADCTriggerType {
//   CONTINUOUS, SOFTWARE_START
// } ADCTriggerType;


// Macro to convert a 12-bit ADC to a float from 0 to 1
#define ADC_TO_FLOAT(i) ((i)/((float)(1<<12)))


// Set ADC trigger type
// @mode CONTINUOUS (**default**) or SOFTWARE_START
// 
// Set the ADC trigger type.
// 
// * Must be called before <adcInit>
// * Calling <analogRead> or <analogReadBatch> should be faster in CONTINUOUS conversion mode, but if precise timing (or Arduino-like behavior) is required, select SOFTWARE_START. Then, the conversion is triggered by <analogRead> or <analogReadBatch>.
// void adcSetMode(ADCTriggerType mode);

// Set ADC sample time
// @sampleTime One of ADC_SampleTime_1Cycles5, ADC_SampleTime_7Cycles5, ADC_SampleTime_13Cycles5, ADC_SampleTime_28Cycles5, ADC_SampleTime_41Cycles5, ADC_SampleTime_55Cycles5 (**default**), ADC_SampleTime_71Cycles5, ADC_SampleTime_239Cycles5. (E.g. the default is 55.5 cycles sampling time)
// 
// Set the ADC sample time.
// 
// * Must be called before <adcInit>
// * This should be seen as a type of filtering
// void adcSetSampleTime(uint8_t sampleTime);

// Initialize ADC channels
// @numChannels number of ADC conversions needed
// @pins Array of pins of size `numChannels`
// 
// This function is a departure from the Arduino API and must be called at the moment. 
// 
// *NOTE:* This API may change to be more Arduino-compatible.
// 
// *FIXME:* Make it unnecessary to call adcInit Idea: just like with the timer system, create a data structure of all 9 or so ADC's, and calling analogRead or analogReadBatch once will add that ADC pin to the list of conversions?
void adcInit();

// void adcInit(int numChannels, const uint8_t *pins);

// Read a single pin
// @pin Pin name to read
// 
// To use,
// 
// * Call <adcSetMode> (or defaults to CONTINUOUS)
// * Call <adcInit> with all the pins required
// * Call <analogRead> to get a new sample
// 
// 3 calls takes ~14 us, 1 call ~5 us. KEEP IN MIND FOR BRUSHLESS CONTROLLER
// 
// @return The 12-bit (0-4096) value of the converted signal, with 0 = 0V, 4096 = 3.3V
uint16_t analogRead(uint8_t pin);

// Read sequentially
// @numChannels Number of channels
// @pins Array of size numChannels, to convert sequentially
// @out Array to store converted values in
// 
// This function performs *numChannels* ADC conversions sequentially.
// 
// The STM32 only has 1 ADC, but with several channels, and consequently, triggering a regular conversion converts all the channels. So, it is more efficient to do the conversion in batch  (taking O(1) time) rather than sequentially (taking O(n) time).
// 
// To use,
// 
// * Call <adcSetMode> (or defaults to CONTINUOUS)
// * Call <adcInit> with all the pins required
// * Call <analogReadBatch> to sequentially sample all the inited ADC channels
// void analogReadBatch(int numChannels, const uint8_t *pins, uint16_t *out);

// cldoc:end-category()

#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus

#endif

