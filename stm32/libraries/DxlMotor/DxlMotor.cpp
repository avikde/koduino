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
  master->sendPacket(id, DXL_CMD_SET_OPEN_LOOP, 4, (uint8_t *)&valToSend);
  master->parserState = DXL_SEARCH_FIRST_FF;
  master->Ser.flushInput();
  lastTX = micros();
  // don't wait here, instead make user call updated() before using this master again

  // // NEW BLOCKING
  // uint32_t tic1 = micros();
  // bool timedout = false;
  // while (!timedout) {
  //   master->listen();
  //   if (master->responseReceived) {
  //     updateStatus(master->pktId, (DxlPacketBLConStatus *)master->packet);
  //     return;
  //   }
  //   uint32_t delt = micros()-tic1;
  //   // FIXME micros bug when bus is running
  //   timedout = (delt > 320 && delt < 1000);
  // }
  // // timed out (no updateStatus called)
  // failures++;
}


void DxlMotor::update2() {
  // TIMEOUT and disable 
  // FIXME micros bug
  bool timedout = (micros() - lastRX > 25000 && micros() - lastRX < 30000);
  // safety
  if (!isnan(prevPos) && timedout) {
    enableFlag = false;
  }

  // BLock here till the update comes back
  timedout = false;
  while (!timedout) {
    master->listen();
    if (master->responseReceived) {
      DxlPacketBLConStatus *status = (DxlPacketBLConStatus *)master->packet;
      if (master->pktId == this->id) {
        prevPos = rawPos = status->position;
        rawCurrent = status->current;
        // timestamp this received packet
        lastRX = micros();
      } else {
        wrongID++;
      }
      return;
    }
    uint32_t delt = micros()-lastTX;
    // FIXME micros bug when bus is running
    timedout = (delt > 320 && delt < 1000);
  }
  // timed out (no updateStatus called)
  failures++;
}


// void DxlMotor::updateStatus(uint8_t id, DxlPacketBLConStatus *status) {
//    // FIXME micros bug
//   bool timedout = (micros() - lastRX > 25000 && micros() - lastRX < 30000);
//   // safety
//   if (!isnan(prevPos) && timedout) {
//     enableFlag = false;
//   }

//   if (id == this->id) {
//     // const float rpusLimit = 50 / ((float)1000000);
//     // float dispLimit = rpusLimit * (micros() - lastRX);
//     // if (!isnan(prevPos)) {
//     //   float absDisplacement = fabsf(fmodf_mpi_pi(status->position - prevPos));
//     //   if (absDisplacement > dispLimit) {
//     //     // this is a pos read error
//     //     return;
//     //   }
//     // }
//     // no pos read error
//     prevPos = rawPos = status->position;
//     rawCurrent = status->current;
//     lastRX = micros();
//   } else {
//     wrongID++;
//   }
// }

