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
#include "DxlMotor.h"

void DxlMotor::init(DxlNode *master_, uint8_t id_, float zero, int8_t dir, float gearRatio) {
  master = master_;
  id = id_;
  driverDirection = 1;
  // Call base class init
  Motor::init(zero, dir, gearRatio);
}

void DxlMotor::sendOpenLoop(float val) {
  // sign reversed
  float valToSend = (enableFlag) ? map(-val, -1, 1, 0.12, 0.88) : 0;
  // clear RX
  master->Ser.flushInput();
  master->sendPacket(id, DXL_CMD_SET_OPEN_LOOP, 4, (uint8_t *)&valToSend);
  lastTX = micros();
  // don't wait here, instead make user call updated() before using this master again
}

bool DxlMotor::updated() {
  while (micros() - lastTX < DXL_TX_TIMEOUT) {
    if (master->listen() == DXL_RX_SUCCESS) {
      if (master->getInstruction() == DXL_STATUS && master->packet[2] == id) {
        DxlPacketBLConStatus *status = (DxlPacketBLConStatus *)master->getPacket();
        rawPos = status->position;
        rawCurrent = status->current;
      }
      delayMicroseconds(10);
      return true;
    }
  }
  // timed out: get rid of anything received
  master->Ser.flushInput();
  return false;
}

