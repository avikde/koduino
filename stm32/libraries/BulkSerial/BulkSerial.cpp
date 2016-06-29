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
#include "BulkSerial.h"

const BulkSerialSettings MBLC_OPENLOG = {PB10, PB11, USART3, DMA1_Channel2, DMA1_Channel3};
const BulkSerialSettings MBLC_RPI = {PA2, PA3, USART2, DMA1_Channel6, DMA1_Channel7};

const uint8_t OPENLOG_ALIGNMENT_WORD[] = {0xaa, 0xbb};

void BulkSerial::begin(uint32_t baud, uint16_t sizeTx, void *bufTx, uint16_t sizeRx, void *bufRx) {
  this->sizeTx = sizeTx;
  this->sizeRx = sizeRx;

  configUSARTPins(bss.USARTx, bss.txPin, bss.rxPin);

  USART_InitTypeDef USART_InitStructure;
  USART_InitStructure.USART_BaudRate = baud;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(bss.USARTx, &USART_InitStructure);

  USART_Cmd(bss.USARTx, ENABLE);

  // this is the same for each chip so far
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

  DMA_InitTypeDef  DMA_InitStructure;

  if (sizeTx > 0) {
    // setup TX
    DMA_DeInit(bss.chTx);
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST; // Transmit
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)bufTx;
    DMA_InitStructure.DMA_BufferSize = sizeTx;
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&bss.USARTx->TDR;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;// or Circular
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_Init(bss.chTx, &DMA_InitStructure);
    
    // Don't enable till a TX is needed

    USART_DMACmd(bss.USARTx, USART_DMAReq_Tx, ENABLE);
  }

  if (sizeRx > 0) {
    // setup RX
    // nvicEnable(bss.irqnRx, 14);

    DMA_DeInit(bss.chRx);
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)bufRx;
    DMA_InitStructure.DMA_BufferSize = sizeRx;
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&bss.USARTx->RDR;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;// or Circular
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_Init(bss.chRx, &DMA_InitStructure);

    // later we can enable callback interrupts when a new message has arrived
    // DMA_ITConfig(bss.chRx, DMA_IT_TC, ENABLE);
    
    DMA_Cmd(bss.chRx, ENABLE);

    USART_DMACmd(bss.USARTx, USART_DMAReq_Rx, ENABLE);
  }
}

void BulkSerial::write() {
  if (!enabled) return;

  DMA_Cmd(bss.chTx, DISABLE);
  DMA_SetCurrDataCounter(bss.chTx, sizeTx);
  DMA_Cmd(bss.chTx, ENABLE);
}

void BulkSerial::initOpenLog(const char *header, const char *fmt) {
  delay(500);
  for (int i = 0; header[i] != 0; i++){
    while (USART_GetFlagStatus(bss.USARTx, USART_FLAG_TXE) == RESET);
    USART_SendData(bss.USARTx, header[i]);
  }
  for (int i = 0; fmt[i] != 0; i++){
    while (USART_GetFlagStatus(bss.USARTx, USART_FLAG_TXE) == RESET);
    USART_SendData(bss.USARTx, fmt[i]);
  }
}
