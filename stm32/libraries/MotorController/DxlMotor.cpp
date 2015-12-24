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
  float valToSend = (enableFlag) ? map(val, -1, 1, 0.1, 0.9) : 0;
  master->sendPacket(id, DXL_CMD_SET_OPEN_LOOP, 4, (uint8_t *)&valToSend);
  uint32_t t = micros();
  while (micros() - t < 1000) {
    if (master->listen()) {
      if (master->getInstruction() == DXL_STATUS) {
        // success = true;
        DxlPacketBLConStatus *status = (DxlPacketBLConStatus *)master->getPacket();
        rawPos = status->position;
        rawCurrent = status->current;
      }
      break;
    }
  }
  // tic = micros() - tic;
}

