
#include "nvic.h"

void nvicEnable(uint8_t irqChannel, uint8_t priority)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  NVIC_InitStructure.NVIC_IRQChannel = irqChannel;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = priority;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}


