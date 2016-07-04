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
#include "RSBus.h"
#include <nvic.h>

// Need some global vars for the slave's ISRs
// If a different board is used these will need to be changed, and also the ISRs below
// (more difficult)
#if defined(STM32F302x8)
// slave
uint8_t RSBUS_DE = PA15;
uint8_t RSBUS_ID = 0;//overwritten by init
USART_TypeDef *RSBUS_USARTx = USART1;
IRQn_Type RSBUS_USART_IRQn = USART1_IRQn;
DMA_Channel_TypeDef *chTx = DMA1_Channel4;
DMA_Channel_TypeDef *chRx = DMA1_Channel5;
uint32_t flagTxTC = DMA1_FLAG_TC4;//not used--interrupts for slave
uint32_t flagRxTC = DMA1_FLAG_TC5;//not used--interrupts for slave
#else
// assume MBLC F303V UART4
uint8_t RSBUS_DE = PA12;
uint8_t RSBUS_ID = RSBUS_MASTER_ID;
USART_TypeDef *RSBUS_USARTx = UART4;
IRQn_Type RSBUS_USART_IRQn = UART4_IRQn;// should not be activated for master
DMA_Channel_TypeDef *chTx = DMA2_Channel5;
DMA_Channel_TypeDef *chRx = DMA2_Channel3;
uint32_t flagTxTC = DMA2_FLAG_TC5;
uint32_t flagRxTC = DMA2_FLAG_TC3;
#endif

DMA_InitTypeDef  DMA_InitStructure_Tx, DMA_InitStructure_Rx;

// checksum adds the lower byte of everything except the first (target address) and the last two (the checksum itself)
#define RSBUS_M2S_PACKET_SIZE 8
typedef struct {
  uint16_t slaveAddr, cmd;
  uint16_t param1[4];
  uint16_t checksum[2];
} __attribute__ ((packed)) RSBusM2S;
#define RSBUS_S2M_PACKET_SIZE 17
typedef struct {
  uint16_t masterAddr, selfAddr, cmd;
  uint16_t param1[4], param2[4], param3[4];
  uint16_t checksum[2];
} __attribute__ ((packed)) RSBusS2M;
uint16_t m2sBuf[RSBUS_M2S_PACKET_SIZE] = {0,0,0,0,0,0,0,0};
uint16_t s2mBuf[RSBUS_S2M_PACKET_SIZE] = {RSBUS_MASTER_ID,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0};

// helper returns sum of bytes to go into checksum as well
static inline uint16_t param2buf(int paramSize, const uint8_t *param, uint16_t *bufParam) {
  uint16_t sum = 0;
  for (int i=0; i<paramSize; ++i) {
    sum += (bufParam[i] = param[i]);
  }
  return sum;
}
// helper returns sum of bytes to go into checksum as well
static inline uint16_t buf2param(int paramSize, const uint16_t *bufParam, uint8_t *param) {
  uint16_t sum = 0;
  for (int i=0; i<paramSize; ++i) {
    sum += (param[i] = (uint8_t)(bufParam[i] & 0xff));
  }
  return sum;
}

// 
void rsBusSetM2S(uint8_t slaveId, uint8_t cmd, float param1) {
  RSBusM2S *buf = (RSBusM2S *)m2sBuf;
  buf->slaveAddr = slaveId;
  bitSet(buf->slaveAddr, 8);

  uint16_t sum = 0;
  sum += (buf->cmd = cmd);
  sum += param2buf(4, (const uint8_t *)&param1, buf->param1);
  param2buf(2, (const uint8_t *)&sum, buf->checksum);
}
bool rsBusGetM2S(uint8_t& cmd, float& param1) {
  RSBusM2S *buf = (RSBusM2S *)m2sBuf;
  uint16_t sum = 0;
  sum += (cmd = (uint8_t)(buf->cmd & 0xff));
  sum += buf2param(4, buf->param1, (uint8_t *)&param1);
  uint16_t checksum = 0;
  buf2param(4, buf->checksum, (uint8_t *)&checksum);
  return (checksum == sum);
}

void rsBusSetS2M(uint8_t cmd, float param1, float param2, float param3) {
  RSBusS2M *buf = (RSBusS2M *)s2mBuf;
  uint16_t sum = RSBUS_ID;
  sum += (buf->cmd = cmd);
  sum += param2buf(4, (const uint8_t *)&param1, buf->param1);
  sum += param2buf(4, (const uint8_t *)&param2, buf->param2);
  sum += param2buf(4, (const uint8_t *)&param3, buf->param3);
  param2buf(2, (const uint8_t *)&sum, buf->checksum);
}

bool rsBusGetS2M(uint8_t& cmd, float& param1, float& param2, float& param3) {
  RSBusS2M *buf = (RSBusS2M *)s2mBuf;
  uint16_t sum = 0;
  sum += (uint8_t)(buf->selfAddr & 0xff);
  sum += (cmd = (uint8_t)(buf->cmd & 0xff));
  sum += buf2param(4, buf->param1, (uint8_t *)&param1);
  sum += buf2param(4, buf->param2, (uint8_t *)&param2);
  sum += buf2param(4, buf->param3, (uint8_t *)&param3);
  uint16_t checksum = 0;
  buf2param(2, buf->checksum, (uint8_t *)&checksum);
  return (checksum == sum);
}


void rsBusInit(uint8_t txPin, uint8_t rxPin, uint8_t slaveId) {
  const uint32_t baud = 9000000;
  const uint8_t priority = 1;
  RSBUS_ID = slaveId;

  digitalWrite(RSBUS_DE, LOW);
  pinMode(RSBUS_DE, OUTPUT);

  configUSARTPins(RSBUS_USARTx, txPin, rxPin);

  // slave needs interrupt for stuff
  if (RSBUS_ID != RSBUS_MASTER_ID) {
    nvicEnable(RSBUS_USART_IRQn, priority);
  }
  
  // Init USART
  USART_OverSampling8Cmd(RSBUS_USARTx, ENABLE);
  USART_InitTypeDef USART_InitStructure;
  USART_InitStructure.USART_BaudRate = baud;
  USART_InitStructure.USART_WordLength = USART_WordLength_9b;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(RSBUS_USARTx, &USART_InitStructure);

  if (RSBUS_ID != RSBUS_MASTER_ID) {
  // 2. Configures the USART address using the USART_SetAddress() function.
  USART_AddressDetectionConfig(RSBUS_USARTx, USART_AddressLength_7b);
  USART_SetAddress(RSBUS_USARTx, RSBUS_ID);
  // 3. Configures the wake up methode (USART_WakeUp_IdleLine or
  // USART_WakeUp_AddressMark) using USART_WakeUpConfig() function only for the
  // slaves.
  USART_MuteModeWakeUpConfig(RSBUS_USARTx, USART_WakeUp_AddressMark);
}
  // 4. Enable the USART using the USART_Cmd() function.
  USART_Cmd(RSBUS_USARTx, ENABLE);
  if (RSBUS_ID != RSBUS_MASTER_ID) {
  // 5. Enter the USART slaves in mute mode using USART_ReceiverWakeUpCmd()
  // function.
  USART_MuteModeCmd(RSBUS_USARTx, ENABLE);
}

  // DMA configuration
  
  if (RSBUS_ID != RSBUS_MASTER_ID) {
    // F301 (grBL): DMA1 channel 4 = USART1_TX, channel 5 = USART1_RX
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    // slave needs interrupts FIXME irqn hardcoded here
    nvicEnable(DMA1_Channel4_IRQn, 3);
    nvicEnable(DMA1_Channel5_IRQn, 2);

    // setup buffers
    s2mBuf[0] = RSBUS_MASTER_ID;// addr = master
    bitSet(s2mBuf[0], 8);
    s2mBuf[1] = RSBUS_ID;

    // Fill out the init structures for both master and slave
    DMA_InitStructure_Tx.DMA_DIR = DMA_DIR_PeripheralDST; // Transmit
    DMA_InitStructure_Tx.DMA_MemoryBaseAddr = (uint32_t)s2mBuf;
    DMA_InitStructure_Tx.DMA_BufferSize = RSBUS_S2M_PACKET_SIZE;
    DMA_InitStructure_Tx.DMA_PeripheralBaseAddr = (uint32_t)&RSBUS_USARTx->TDR;
    DMA_InitStructure_Tx.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure_Tx.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure_Tx.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure_Tx.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure_Tx.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure_Tx.DMA_Priority = DMA_Priority_High;

    DMA_InitStructure_Rx.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure_Rx.DMA_MemoryBaseAddr = (uint32_t)m2sBuf;
    DMA_InitStructure_Rx.DMA_BufferSize = RSBUS_M2S_PACKET_SIZE;
    DMA_InitStructure_Rx.DMA_PeripheralBaseAddr = (uint32_t)&RSBUS_USARTx->RDR;
    DMA_InitStructure_Rx.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure_Rx.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure_Rx.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure_Rx.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure_Rx.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStructure_Rx.DMA_Priority = DMA_Priority_High;

    // connect these for master as well
    USART_DMACmd(RSBUS_USARTx, USART_DMAReq_Tx, ENABLE);
    USART_DMACmd(RSBUS_USARTx, USART_DMAReq_Rx, ENABLE);

    // setup TX
    DMA_DeInit(chTx);
    DMA_Init(chTx, &DMA_InitStructure_Tx);
    DMA_ITConfig(chTx, DMA_IT_TC, ENABLE);
    // Don't enable till a TX is needed

    // setup RX
    DMA_DeInit(chRx);
    DMA_Init(chRx, &DMA_InitStructure_Rx);
    DMA_ITConfig(chRx, DMA_IT_TC, ENABLE);
    DMA_Cmd(chRx, ENABLE);
  } else {
    // F303V (MBLC J9): DMA2 channel 3 = UART4_RX, channel 5 = UART4_TX
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);
    // master will use flags, no interrupts needed

    DMA_InitStructure_Rx.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure_Rx.DMA_MemoryBaseAddr = (uint32_t)s2mBuf;
    DMA_InitStructure_Rx.DMA_BufferSize = RSBUS_S2M_PACKET_SIZE;
    DMA_InitStructure_Rx.DMA_PeripheralBaseAddr = (uint32_t)&RSBUS_USARTx->RDR;
    DMA_InitStructure_Rx.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure_Rx.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure_Rx.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure_Rx.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    // TODO circular for slave but what about for master?
    DMA_InitStructure_Rx.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStructure_Rx.DMA_Priority = DMA_Priority_High;
    USART_DMACmd(RSBUS_USARTx, USART_DMAReq_Rx, ENABLE);

    DMA_DeInit(chRx);
    DMA_Init(chRx, &DMA_InitStructure_Rx);
    DMA_Cmd(chRx, ENABLE);

    DMA_InitStructure_Tx.DMA_DIR = DMA_DIR_PeripheralDST; // Transmit
    DMA_InitStructure_Tx.DMA_MemoryBaseAddr = (uint32_t)m2sBuf;
    DMA_InitStructure_Tx.DMA_BufferSize = RSBUS_M2S_PACKET_SIZE;
    DMA_InitStructure_Tx.DMA_PeripheralBaseAddr = (uint32_t)&RSBUS_USARTx->TDR;
    DMA_InitStructure_Tx.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure_Tx.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure_Tx.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure_Tx.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure_Tx.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure_Tx.DMA_Priority = DMA_Priority_High;

    USART_DMACmd(RSBUS_USARTx, USART_DMAReq_Tx, ENABLE);

    // setup buffers?
  }
}

bool rsBusMasterPing() {
  DMA_Cmd(chTx, DISABLE);
  DMA_DeInit(chTx);
  DMA_Init(chTx, &DMA_InitStructure_Tx);

  // give time for DE change
  digitalWrite(RSBUS_DE, HIGH);

  DMA_Cmd(chTx, ENABLE);

  // wait till TX finishes, then deassert DE
  while (!DMA_GetFlagStatus(flagTxTC)); // wait till TC
  DMA_ClearFlag(flagTxTC);
  while (USART_GetFlagStatus(RSBUS_USARTx, USART_FLAG_TC) == RESET);
  digitalWrite(RSBUS_DE, LOW);

  // Wait for slave response
  uint32_t tic = micros();

  while (!DMA_GetFlagStatus(flagRxTC) && micros() - tic < 50);
  DMA_ClearFlag(flagRxTC);
  return !(micros() - tic > 50);
}

// SHOULD NOT BE USED ANY MORE
void rsBusTxPolling() {
  // set to TX mode
  digitalWrite(RSBUS_DE, HIGH);

  uint16_t *pBuf = (uint16_t *)&m2sBuf;
  for (int i=0; i<RSBUS_M2S_PACKET_SIZE; ++i) {
    while (USART_GetFlagStatus(RSBUS_USARTx, USART_FLAG_TXE) == RESET);
    USART_SendData(RSBUS_USARTx, pBuf[i]);
  }
  while (USART_GetFlagStatus(RSBUS_USARTx, USART_FLAG_TC) == RESET);
  digitalWrite(RSBUS_DE, LOW);
}

// slave uses this command
void rsBusTxDMA() {
  // give time for DE change
  digitalWrite(RSBUS_DE, HIGH);
  DMA_Cmd(chTx, DISABLE);
  DMA_SetCurrDataCounter(chTx, (RSBUS_ID == RSBUS_MASTER_ID) ? RSBUS_M2S_PACKET_SIZE : RSBUS_S2M_PACKET_SIZE);
  DMA_Cmd(chTx, ENABLE);
  // interrupt will deassert
}

// Slave ISRs
extern "C" {

#if defined(STM32F302x8)

// interrupts for TX TC (to de-assert DE)
void DMA1_Channel4_IRQHandler() {
  if (DMA_GetITStatus(DMA1_IT_TC4)) {
    DMA_ClearITPendingBit(DMA1_IT_TC4);
    // enable usart TC interrupt (wait till last byte has shifted out)
    USART_ITConfig(RSBUS_USARTx, USART_IT_TC, ENABLE);
  }
}
// de-assert DE
void USART1_IRQHandler() {
  if(USART_GetITStatus(RSBUS_USARTx, USART_IT_TC) != RESET) {
    digitalWrite(RSBUS_DE, LOW);
    USART_ClearITPendingBit(RSBUS_USARTx, USART_IT_TC);
    USART_ITConfig(RSBUS_USARTx, USART_IT_TC, DISABLE);
  }
}

// interrupt for RX complete (respond to master packet)
void DMA1_Channel5_IRQHandler() {
  if (DMA_GetITStatus(DMA1_IT_TC5)) {
    DMA_ClearITPendingBit(DMA1_IT_TC5);
    // send response
    rsBusTxDMA();
    // process incoming packet in m2sBuf

    // digitalWrite(PA6, TOGGLE);
  }
}
#endif
} // extern "C"
