
#include "USARTClass.h"
#include "nvic.h"
#include "pins.h"
#include "system_clock.h"

// RingBuffer helper
inline void ringBufferStore(unsigned char c, RingBuffer *buffer) {
  unsigned i = (unsigned int)(buffer->head + 1) % SERIAL_BUFFER_SIZE;

	if (i != buffer->tail) {
		buffer->buffer[buffer->head] = c;
		buffer->head = i;
	}
}

// Initialize Class Variables //////////////////////////////////////////////////
USART_InitTypeDef USARTClass::USART_InitStructure;
// bool USARTClass::USARTSerial_Enabled = false;

// Constructors ////////////////////////////////////////////////////////////////

USARTClass::USARTClass(USARTInfo *usartMapPtr) {
  usartMap = usartMapPtr;

  usartMap->rxBuf = &_rxBuf;
  usartMap->txBuf = &_txBuf;

  memset(&_rxBuf, 0, sizeof(RingBuffer));
  memset(&_txBuf, 0, sizeof(RingBuffer));

  // transmitting = false;
  usartMap->rxCallback = NULL;
}

void USARTClass::init(uint32_t baud, uint32_t wordLength, uint32_t parity, uint32_t stopBits) {
  // Enable interrupt (for RXNE)
  nvicEnable(usartMap->irqChannel, 0);
  
  // Init USART
  USART_InitStructure.USART_BaudRate = baud;
  USART_InitStructure.USART_WordLength = wordLength;
  USART_InitStructure.USART_Parity = parity;
  USART_InitStructure.USART_StopBits = stopBits;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(usartMap->USARTx, &USART_InitStructure);

  USART_Cmd(usartMap->USARTx, ENABLE);
}

// Public Methods //////////////////////////////////////////////////////////////

void USARTClass::begin(uint32_t baud, uint8_t config) {
  // Always AF7 for USART
  pinModeAlt(usartMap->txPin, GPIO_OType_PP, GPIO_PuPd_UP, 7);
  pinModeAlt(usartMap->rxPin, GPIO_OType_PP, GPIO_PuPd_UP, 7);

  switch (config) {
    case SERIAL_8N1: init(baud, USART_WordLength_8b, USART_Parity_No, USART_StopBits_1);
    break;
    case SERIAL_8N2: init(baud, USART_WordLength_8b, USART_Parity_No, USART_StopBits_2);
    break;
    case SERIAL_7E1: init(baud, USART_WordLength_8b, USART_Parity_Even, USART_StopBits_1);
    break;
    case SERIAL_8E1: init(baud, USART_WordLength_9b, USART_Parity_Even, USART_StopBits_1);
    break;
    case SERIAL_7E2: init(baud, USART_WordLength_8b, USART_Parity_Even, USART_StopBits_2);
    break;
    case SERIAL_8E2: init(baud, USART_WordLength_9b, USART_Parity_Even, USART_StopBits_2);
    break;
    case SERIAL_7O1: init(baud, USART_WordLength_8b, USART_Parity_Odd, USART_StopBits_1);
    break;
    case SERIAL_8O1: init(baud, USART_WordLength_9b, USART_Parity_Odd, USART_StopBits_1);
    break;
    case SERIAL_7O2: init(baud, USART_WordLength_8b, USART_Parity_Odd, USART_StopBits_2);
    break;
    case SERIAL_8O2: init(baud, USART_WordLength_9b, USART_Parity_Odd, USART_StopBits_2);
    break;
  }

  // USART interrupts
  // USART_ITConfig(USARTx, USART_IT_TXE, ENABLE);
  USART_ClearFlag(usartMap->USARTx, USART_FLAG_RXNE);
  USART_ClearITPendingBit(usartMap->USARTx, USART_IT_RXNE);
  USART_ITConfig(usartMap->USARTx, USART_IT_RXNE, ENABLE);
}

void USARTClass::end() {
  // wait for transmission to end
  flush();
  USART_ITConfig(usartMap->USARTx, USART_IT_RXNE, DISABLE);
  USART_ITConfig(usartMap->USARTx, USART_IT_TXE, DISABLE);
	USART_Cmd(usartMap->USARTx, DISABLE);
  NVIC_DisableIRQ(usartMap->irqChannel);
	// clear any received data
	_rxBuf.head = _rxBuf.tail;
  // null ring buffer pointers
  usartMap->txBuf = NULL;
  usartMap->rxBuf = NULL;
}

void USARTClass::setPins(uint8_t tx, uint8_t rx) {
  usartMap->txPin = tx;
  usartMap->rxPin = rx;
}

int USARTClass::available(void) {
	return (int)(SERIAL_BUFFER_SIZE + _rxBuf.head - _rxBuf.tail) % SERIAL_BUFFER_SIZE;
}

int USARTClass::peek(void) {
	if (_rxBuf.head == _rxBuf.tail)
		return -1;
	else
		return _rxBuf.buffer[_rxBuf.tail];
}

int USARTClass::read(void) {
	// if the head isn't ahead of the tail, we don't have any characters
	if (_rxBuf.head == _rxBuf.tail)
		return -1;
	else {
		unsigned char c = _rxBuf.buffer[_rxBuf.tail];
		_rxBuf.tail = (unsigned int)(_rxBuf.tail + 1) % SERIAL_BUFFER_SIZE;
		return c;
	}
}

bool USARTClass::writeComplete() {
  return (_txBuf.head == _txBuf.tail);
}

void USARTClass::flush() {
  // wait for transmission of outgoing data
  while (_txBuf.head != _txBuf.tail);
	// Loop until last frame transmission complete
	while (USART_GetFlagStatus(usartMap->USARTx, USART_FLAG_TC) == RESET);
}

size_t USARTClass::write(uint8_t c) {
  // HACK: to stop bootloading problems, don't output to Serial1 for the first second
  if (usartMap->USARTx == USART1 && millis() < 1000)
    return 0;

  int i = (_txBuf.head + 1) % SERIAL_BUFFER_SIZE;

  // Buffer overrun protection?
  while (i == _txBuf.tail || ((__get_PRIMASK() & 1) && _txBuf.head != _txBuf.tail)) {
    // Interrupts are on but they are not being serviced because this was called from a higher priority interrupt
    if (USART_GetITStatus(usartMap->USARTx, USART_IT_TXE) && USART_GetFlagStatus(usartMap->USARTx, USART_FLAG_TXE)) {
      // protect for good measure
      USART_ITConfig(usartMap->USARTx, USART_IT_TXE, DISABLE);
      // Write out a byte
      USART_SendData(usartMap->USARTx,  _txBuf.buffer[_txBuf.tail++]);
      _txBuf.tail %= SERIAL_BUFFER_SIZE;
      // unprotect
      USART_ITConfig(usartMap->USARTx, USART_IT_TXE, ENABLE);
    }
  }

  _txBuf.buffer[_txBuf.head] = c;
  _txBuf.head = i;

  USART_ITConfig(usartMap->USARTx, USART_IT_TXE, ENABLE);

	return 1;
}

void USARTClass::attachInterrupt(ByteFunc f) {
  usartMap->rxCallback = f;
}

void USARTClass::detachInterrupt() {
  usartMap->rxCallback = NULL;
}

// bool USARTClass::isEnabled() {
//  return USARTSerial_Enabled;
// }

// ============================================================================
// Custom USART interrupt handler
// ============================================================================

extern "C"{

// Shared Interrupt Handler for USART2/Serial1 and USART1/Serial2
// WARNING: This function MUST remain reentrance compliant -- no local static variables etc.
void wirishUSARTInterruptHandler(USARTInfo *usartMap)
{
  if(USART_GetITStatus(usartMap->USARTx, USART_IT_RXNE) != RESET) {
    // Read byte from the receive data register
    uint8_t c = USART_ReceiveData(usartMap->USARTx);
    // If interrupt is attached then don't use the RingBuffer
    if (usartMap->rxCallback != NULL)
      usartMap->rxCallback(c);
    else
      ringBufferStore(c, usartMap->rxBuf);
  }

  if(USART_GetITStatus(usartMap->USARTx, USART_IT_TXE) != RESET) {
    // Write byte to the transmit data register
    if (usartMap->txBuf->head == usartMap->txBuf->tail) {
      // Buffer empty, so disable the USART Transmit interrupt
      USART_ITConfig(usartMap->USARTx, USART_IT_TXE, DISABLE);
    }
    else {
      // There is more data in the output buffer. Send the next byte
      USART_SendData(usartMap->USARTx, usartMap->txBuf->buffer[usartMap->txBuf->tail++]);
      usartMap->txBuf->tail %= SERIAL_BUFFER_SIZE;
    }
  }
}

} // extern "C"
