
#include "Wire.h"

// 1000 gives ~600us block in the requestFrom function when the slave is not connected. 10000 gives ~6000us
#define WIRE_TIMEOUT 1000 

// Constructors ////////////////////////////////////////////////////////////////

TwoWire::TwoWire(I2C_TypeDef *I2Cx) : I2Cx(I2Cx) {
	setSpeed(WIRE_100K);
}

// Public Methods //////////////////////////////////////////////////////////////

//setSpeed() should be called before begin() else default to 100KHz
void TwoWire::setSpeed(WireClockSpeed clockSpeed) {
  this->clockSpeed = clockSpeed;
}

void TwoWire::stretchClock(bool stretch) {
  I2C_StretchClockCmd(I2Cx, (stretch == true) ? ENABLE : DISABLE);
}

void TwoWire::begin(void) {
  if (I2Cx == I2C1) {
    SYSCFG_I2CFastModePlusConfig(SYSCFG_I2CFastModePlus_I2C1, ENABLE);
    RCC_I2CCLKConfig(RCC_I2C1CLK_SYSCLK);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);

    // setPins?
    pinModeAlt(PA14, GPIO_OType_OD, GPIO_PuPd_NOPULL, 4);
    pinModeAlt(PA15, GPIO_OType_OD, GPIO_PuPd_NOPULL, 4);
  }
  else if (I2Cx == I2C2) {
    // SYSCFG_I2CFastModePlusConfig(SYSCFG_I2CFastModePlus_I2C2, ENABLE);
    RCC_I2CCLKConfig(RCC_I2C2CLK_SYSCLK);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);

    // setPins?
    pinModeAlt(PF7, GPIO_OType_OD, GPIO_PuPd_NOPULL, 4);
    pinModeAlt(PF1, GPIO_OType_OD, GPIO_PuPd_NOPULL, 4);
  }

  I2C_InitTypeDef I2C_InitStructure;


  I2C_DeInit(I2Cx);
  I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;  
  I2C_InitStructure.I2C_AnalogFilter = I2C_AnalogFilter_Enable;
  I2C_InitStructure.I2C_DigitalFilter = 0x00;
  I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
  I2C_InitStructure.I2C_Timing = clockSpeed; 

  I2C_Init(I2Cx, &I2C_InitStructure);  
  I2C_Cmd(I2Cx, ENABLE);
}

void TwoWire::begin(uint8_t address) {
  // TODO: slave mode
}

void TwoWire::begin(int address) {
  begin((uint8_t)address);
}

// Arduino API ===============================================================


void TwoWire::beginTransmission(uint8_t address) {
  transmitting = true;
  // set address of targeted slave
  txAddress = address;
  // reset tx buffer iterator vars
  // txBufferIndex = 0;
  txBufferLength = 0;
  txBufferFull = false;
}

uint8_t TwoWire::endTransmission(bool stop) {
  // Returns byte, which indicates the status of the transmission:
  // 0:success
  // 1:data too long to fit in transmit buffer
  // 2:received NACK on transmit of address
  // 3:received NACK on transmit of data
  // 4:other error
  static uint32_t timeout;

  if (txBufferFull)
    return 1;

  //Wait until I2C isn't busy
  timeout = WIRE_TIMEOUT;
  while (I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY) == SET) {
    if ((timeout--) == 0) return 4;
  }

  I2C_TransferHandling(I2Cx, txAddress << 1, txBufferLength, (stop) ? I2C_AutoEnd_Mode : I2C_SoftEnd_Mode, I2C_Generate_Start_Write);

  for (int i=0; i<txBufferLength; ++i) {
    //Again, wait until the transmit interrupted flag is set
    timeout = WIRE_TIMEOUT;
    while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_TXIS) == RESET) {
      if ((timeout--) == 0) return (i==0) ? 2 : 3;
    }

    //Send the value you wish you write to the register
    I2C_SendData(I2Cx, txBuffer[i]);
  }

  if (stop) {
    //Wait for the stop flag to be set indicating
    //a stop condition has been sent
    timeout = WIRE_TIMEOUT;
    while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_STOPF) == RESET) {
      if ((timeout--) == 0) return 4;
    }
    //Clear the stop flag for the next potential transfer
    I2C_ClearFlag(I2Cx, I2C_FLAG_STOPF);
  } else {
    //Wait until transfer is complete!
    timeout = WIRE_TIMEOUT;
    while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_TC) == RESET) {
      if ((timeout--) == 0) return 4;
    }
  }

  return 0;
}

uint8_t TwoWire::requestFrom(uint8_t address, uint8_t quantity, bool stop) {
  static uint32_t timeout;

  if (quantity > WIRE_BUFFER_LENGTH)
    quantity = WIRE_BUFFER_LENGTH;

  rxBufferIndex = rxBufferLength = 0;

  //As per, start another transfer, we want to read DCnt
  //amount of bytes. Generate a start condition and
  //indicate that we want to read.
  I2C_TransferHandling(I2Cx, address << 1, quantity, (stop) ? I2C_AutoEnd_Mode : I2C_SoftEnd_Mode, I2C_Generate_Start_Read);

  //Read in DCnt pieces of data
  for (int i = 0; i<quantity; ++i) {
    //Wait until the RX register is full of luscious data!
    timeout = WIRE_TIMEOUT;
    while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_RXNE) == RESET) {
      if ((timeout--) == 0) {
        rxBufferLength = i;
        return i;
      }
    }
    //If we're only reading one byte, place that data direct into the 
    //SingleData variable. If we're reading more than 1 piece of data 
    //store in the array "Data" (a pointer from main)     
    rxBuffer[i] = I2C_ReceiveData(I2Cx);
  }

  if (stop) {
    //Wait for the stop flag to be set indicating
    //a stop condition has been sent
    timeout = WIRE_TIMEOUT;
    while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_STOPF) == RESET) {
      if ((timeout--) == 0) break;
    }
    //Clear the stop flag for the next potential transfer
    I2C_ClearFlag(I2Cx, I2C_FLAG_STOPF);
  }

  rxBufferLength = quantity;
  return rxBufferLength;
}

// must be called in:
// slave tx event callback
// or after beginTransmission(address)
size_t TwoWire::write(uint8_t data) {
  if (transmitting) {
    // in master transmitter mode
    // don't bother if buffer is full
    if (txBufferLength >= WIRE_BUFFER_LENGTH) {
      txBufferFull = true;
      return 0;
    }
    // put byte in tx buffer
    txBuffer[txBufferLength] = data;
    ++txBufferLength;
  } else{
    // in slave send mode
    // reply to master
    // twi_transmit(&data, 1);
  }
  return 1;
}

// must be called in:
// slave tx event callback
// or after beginTransmission(address)
size_t TwoWire::write(const uint8_t *data, size_t quantity) {
  if (transmitting) {
    // in master transmitter mode
    for(size_t i = 0; i < quantity; ++i){
      write(data[i]);
    }
  } else {
    // in slave send mode
    // reply to master
    // twi_transmit(data, quantity);
  }
  return quantity;
}

// must be called in:
// slave rx event callback
// or after requestFrom(address, numBytes)
int TwoWire::available(void) {
  return rxBufferLength - rxBufferIndex;
}

// must be called in:
// slave rx event callback
// or after requestFrom(address, numBytes)
int TwoWire::read(void) {
  int value = -1;
  
  // get each successive byte on each call
  if(rxBufferIndex < rxBufferLength){
    value = rxBuffer[rxBufferIndex];
    ++rxBufferIndex;
  }

  return value;
}

// must be called in:
// slave rx event callback
// or after requestFrom(address, numBytes)
int TwoWire::peek(void) {
  int value = -1;
  
  if(rxBufferIndex < rxBufferLength){
    value = rxBuffer[rxBufferIndex];
  }

  return value;
}

void TwoWire::flush(void) {
  // TODO: to be implemented.

}

// sets function called on slave write
void TwoWire::onReceive( void (*function)(int) ) {
  user_onReceive = function;
}

// sets function called on slave read
void TwoWire::onRequest( void (*function)(void) ) {
  user_onRequest = function;
}

// Preinstantiate Objects //////////////////////////////////////////////////////

TwoWire Wire(I2C1);
// TwoWire Wire2(I2C2);
