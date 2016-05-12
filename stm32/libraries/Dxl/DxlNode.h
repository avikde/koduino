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
#ifndef DxlNode_h
#define DxlNode_h

#include <Arduino.h>

#define DXL_RX_SIZE 32

// enum DxlRxStatus {
//   DXL_RX_SUCCESS = 0,
//   DXL_RX_ID_WRONG = 1,
//   DXL_RX_BAD_CHECKSUM = 2,
//   DXL_RX_BAD_LENGTH = 3,
//   DXL_RX_WAITING = 4
// };

// Configuration commands
#define DXL_STATUS 0x00 // slave return packet has this as instruction byte
#define DXL_CMD_SET_ID 0x01 // 1 byte
#define DXL_CMD_CALIBRATE 0x02 // 0
// other configuration...

// Motor control commands
#define DXL_CMD_ENABLE 0x10 // 1 byte (true or false)
#define DXL_CMD_SET_OPEN_LOOP 0x12 // 1 float = 4 bytes
// ...set position etc.

// the bus is forced to release after this many us
#define DXL_TX_TIMEOUT 300
// this is how long the master waits to get a response from the slave
#define DXL_RX_TIMEOUT 500

// Return packet types
typedef struct {
  float position, current;
} __attribute__ ((packed)) DxlPacketBLConStatus;

typedef void (*PacketReceivedCallback)(uint8_t, uint8_t, volatile uint8_t *);

enum DxlParserState {
  DXL_SEARCH_FIRST_FF=0,
  DXL_SEARCH_SECOND_FF,
  DXL_SEARCH_ID,
  DXL_SEARCH_LENGTH,
  DXL_SEARCH_COMMAND,
  DXL_GET_PARAMETERS
};

class DxlNode {
public:
  // members
  const uint8_t DE, myAddress;
  USARTClass& Ser;
  const bool isMaster;
  
  // incoming packet
  volatile uint8_t packet[DXL_RX_SIZE];
  // packet parser variables
  volatile uint8_t nParams, pktId, pktCmd, paramsSeen, rxChecksum, txChecksum;
  volatile DxlParserState parserState;
  // callback to call when a good packet is received
  PacketReceivedCallback packetReceived;
  volatile bool responseReceived;

  // functions
  DxlNode(uint8_t rts, USARTClass& ser, uint8_t myAddress) : DE(rts), myAddress(myAddress), Ser(ser), isMaster(false) {}
  DxlNode(uint8_t rts, USARTClass& ser) : DE(rts), myAddress(0), Ser(ser), isMaster(true) {}

  // for slave, this makes the packet parsing part of the Serial RXNE interrupt
  // for master, pass NULL. this uses the standard Serial RX and master needs to call parseRX()
  void init(PacketReceivedCallback cb);
  // instErr can be instruction (instruction packet) or error code (status packet)
  // params is of length N
  // wait = true => waits till TX finished and puts back in RX mode
  // wait = false => returns immediately

  // void sendPacket(uint8_t id, uint8_t instErr, uint8_t N, uint8_t *params, bool wait);
  inline void sendPacket(uint8_t id, uint8_t instErr, uint8_t N, uint8_t *params) {
    if (isMaster)
      responseReceived = false;
    
    uint8_t checksum = 0;

    // set to TX mode. TC interrupt sets back.
    digitalWrite(DE, HIGH);

    // packet = 0xFF,0xFF,ID,N+2,INST_ERR,<N BYTES>,CHECKSUM
    Ser._txBuf.tail = 0;
    Ser._txBuf.buffer[0] = Ser._txBuf.buffer[1] = 0xff;
    checksum += (Ser._txBuf.buffer[2] = id);
    checksum += (Ser._txBuf.buffer[3] = N+2);
    checksum += (Ser._txBuf.buffer[4] = instErr);
    for (uint8_t i=0; i<N; ++i) {
      checksum += (Ser._txBuf.buffer[5+i] = params[i]);
    }
    Ser._txBuf.buffer[N+5] = ~checksum;
    txChecksum = Ser._txBuf.buffer[N+5];
    // set head past the end
    Ser._txBuf.head = N+6;
    // start interrupts
    USART_ITConfig(Ser.usartMap->USARTx, USART_IT_TXE, ENABLE);
    USART_ITConfig(Ser.usartMap->USARTx, USART_IT_TC, ENABLE);
  }

  inline void parseRX(uint8_t c) {
    // packet = 0xFF,0xFF,ID,N+2,INST_ERR,<N BYTES>,CHECKSUM
    switch (parserState) {

      case DXL_SEARCH_FIRST_FF:
        if (c == 0xFF)
          parserState = DXL_SEARCH_SECOND_FF;
        break;

      case DXL_SEARCH_SECOND_FF:
        if (c == 0xFF) {
          parserState = DXL_SEARCH_ID;
          rxChecksum = 0;
        } else
          parserState = DXL_SEARCH_FIRST_FF;
        break;

      case DXL_SEARCH_ID:
        rxChecksum += (pktId = c);
        parserState = DXL_SEARCH_LENGTH;
        break;

      case DXL_SEARCH_LENGTH:
        if ((isMaster || (pktId == myAddress)) && c < DXL_RX_SIZE) {
          nParams = c-2;
          rxChecksum += c;
          paramsSeen = 0;
          parserState = DXL_SEARCH_COMMAND;
        } else
          parserState = DXL_SEARCH_FIRST_FF;
        break;

      case DXL_SEARCH_COMMAND:
        rxChecksum += (pktCmd = c);
        parserState = DXL_GET_PARAMETERS;
        break;

      case DXL_GET_PARAMETERS:
        if (paramsSeen >= nParams) {
          if (c == (uint8_t)~rxChecksum) {
            // callback
            if (isMaster)
              responseReceived = true;
            if (packetReceived != NULL) {
              packetReceived(pktId, pktCmd, packet);
            }
            parserState = DXL_SEARCH_FIRST_FF;
          } else {
            parserState = (c == 0xFF) ? DXL_SEARCH_SECOND_FF : DXL_SEARCH_FIRST_FF;
          }
        } else {
          rxChecksum += (packet[paramsSeen++] = c);
        }
        break;

      default:
        break;
    }
  }

  inline void listen() {
    while (Ser.available()) {
      parseRX(Ser.read());
    }
  }

  // // checks if TX complete, if so sets to RX mode
  // bool completeTX();

  // // Update function that should be called as often as possible
  // DxlRxStatus listen();
  // DxlRxStatus checkPacket();

  // // functions on the latest packet
  // uint8_t getInstruction() const { return packet[4]; }
  // const void * getPacket() const { return (const void *)&packet[5]; }

  // void setTX();
  // void setRX(bool force=false);
// protected:
  // uint8_t writeByte(uint8_t c);

  // uint32_t txTime;
};


#endif
