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
#ifndef RSBus_h
#define RSBus_h

#include <Arduino.h>

// highest 7-bit number reserved for master ID (slaves can have ID 0--126)
#define RSBUS_MASTER_ID 0x7F

#define RSBUS_CMD_STATUS 0x00 // slave return packet has this as instruction byte
#define RSBUS_CMD_SET_OPEN_LOOP 0x12 // 1 float = 4 bytes

// hard to encapsulate in a class yet, since there are callbacks from C ISRs
// for now only works on gRBL F301 (slave) and MBLC F303V master

/**
 * @brief Init
 * @details [long description]
 * 
 * @param txPin [description]
 * @param rxPin [description]
 * @param slaveid [description]
 */
void rsBusInit(uint8_t txPin, uint8_t rxPin, uint8_t slaveid);

/**
 * @brief Send the data. Does not block for slave, uses flags to block for master
 * @details Example:
 * 
 * Slave:
 * rsBusInit(..., ..., SLAVE_ID) in setup()
 * rsBusSetS2M(RSBUS_STATUS, pos, cur, vel) in loop() or timer interrupt
 * if (rsBusGetM2S(cmd, pwmdes))...TODO check last good rx time
 * 
 * Master:
 * rsBusInit(..., ..., RSBUS_MASTER_ID) in setup()
 * in loop() or timer interrupt:
 * rsBusSetM2S(slaveId, RSBUS_CMD_SET_OPEN_LOOP, pwmdes)
 * rsBusTxDMA();//waits till RX from slave or timeout
 * if (rsBusGetS2M(cmd, pos, cur, vel))
 * 
 */
bool rsBusMasterPing();

// FOR TESTING ONLY
void rsBusTxPolling();
void rsBusTxDMA();//slave

// Packet stuff

void rsBusSetM2S(uint8_t slaveId, uint8_t cmd, float param1);
bool rsBusGetM2S(uint8_t& cmd, float& param1);

void rsBusSetS2M(uint8_t cmd, float param1, float param2, float param3);
bool rsBusGetS2M(uint8_t& cmd, float& param1, float& param2, float& param3);

extern uint16_t s2mBuf[];
extern uint16_t m2sBuf[];

extern DMA_InitTypeDef  DMA_InitStructure_Tx;

#endif
