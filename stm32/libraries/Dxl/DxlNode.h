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

#define DXL_MAX_PACKET_SIZE 32

enum DxlRxStatus {
  DXL_RX_SUCCESS = 0,
  DXL_RX_ID_WRONG = 1,
  DXL_RX_BAD_CHECKSUM = 2,
  DXL_RX_BAD_LENGTH = 3,
  DXL_RX_WAITING = 4
};

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


class DxlNode {
public:
  // members
  const uint8_t DE, myAddress;
  USARTClass& Ser;
  const bool isMaster;
  
  // incoming packet
  uint8_t packet[DXL_MAX_PACKET_SIZE];

  // functions
  DxlNode(uint8_t rts, USARTClass& ser, uint8_t myAddress) : DE(rts), myAddress(myAddress), Ser(ser), isMaster(false), txTime(0) {}
  DxlNode(uint8_t rts, USARTClass& ser) : DE(rts), myAddress(0), Ser(ser), isMaster(true), txTime(0) {}


  void init();
  // instErr can be instruction (instruction packet) or error code (status packet)
  // params is of length N
  // wait = true => waits till TX finished and puts back in RX mode
  // wait = false => returns immediately
  void sendPacket(uint8_t id, uint8_t instErr, uint8_t N, uint8_t *params, bool wait);
  void sendPacket(uint8_t id, uint8_t instErr, uint8_t N, uint8_t *params) {
    sendPacket(id, instErr, N, params, true);
  }
  // checks if TX complete, if so sets to RX mode
  bool completeTX();

  // Update function that should be called as often as possible
  DxlRxStatus listen();
  DxlRxStatus checkPacket();

  // functions on the latest packet
  uint8_t getInstruction() const { return packet[4]; }
  const void * getPacket() const { return (const void *)&packet[5]; }

  void setTX();
  void setRX();
protected:
  uint8_t writeByte(uint8_t c);

  uint32_t txTime;
};


#endif
