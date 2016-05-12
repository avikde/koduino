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
#include "DxlNode.h"

void DxlNode::init(PacketReceivedCallback cb) {
  Ser.begin(1000000);
  digitalWrite(DE, LOW);
  pinMode(DE, OUTPUT);
  // new
  Ser.attachBus(this);
  // if this is a slave, RX processing happens in an ISR, and it calls this callback when done;. If master, then that happens from non-interrupt code.
  if (cb != NULL) {
    packetReceived = cb;
  }
  // packet parser variables
  parserState = DXL_SEARCH_FIRST_FF;
  nParams = 0;
  pktId = 0xff;
  pktCmd = 0;
  paramsSeen = 0;
  rxChecksum = 0;
}

// DxlRxStatus DxlNode::checkPacket() {
//   // check checksum
//   uint8_t checksum = 0;
//   uint8_t len = packet[3];
//   for (int i=2; i<len+3; ++i) {
//     checksum += packet[i];
//   }
//   checksum = ~checksum;
//   if (packet[len+3] != checksum)
//     return DXL_RX_BAD_CHECKSUM;

//   // good packet received
//   if (isMaster) {
//     return DXL_RX_SUCCESS;
//   } else {
//     // slave should only respond if addressed
//     if (packet[2] != myAddress)
//       return DXL_RX_ID_WRONG;
//     else
//       return DXL_RX_SUCCESS;
//   }
// }

// // both master/slave
// DxlRxStatus DxlNode::listen() {
//   // if there is stuff in the RX buffer that doesn't start with 0xffff, get rid of them
//   while (Ser.available() >= 2 && !(Ser.peekAt(0) == 0xff && Ser.peekAt(1) == 0xff))
//     Ser.read();

//   // the smallest packet size
//   if (Ser.available() >= 4) {
//     // Per protocol, packet[3] = N+2, whereas total bytes = N+6
//     uint8_t len = Ser.peekAt(3) + 4;
//     if (len > DXL_MAX_PACKET_SIZE) {
//       // packet is invalid? pop off the top byte and try again
//       Ser.read();
//       return DXL_RX_BAD_LENGTH;
//     }

//     if (Ser.available() >= len) {
//       // read this packet
//       for (int i=0; i<len; ++i) {
//         packet[i] = Ser.read();
//       }
//       // check this packet
//       return checkPacket();
//     }
//   }
//   return DXL_RX_WAITING;
// }

// void DxlNode::setTX() {
//   digitalWrite(DE, HIGH);
//   delayMicroseconds(1);
// }

// void DxlNode::setRX(bool force) {
//   // both of these ensure the out buffer is empty
//   if (force)
//     Ser.stopTX();
//   else
//     Ser.flush();
//   delayMicroseconds(1);
//   digitalWrite(DE, LOW);
//   delayMicroseconds(1);
// }

// bool DxlNode::completeTX() {
//   // implement a timeout here: if DXL_TX_TIMEOUT us have elapsed since the packet sending started (no idea why this would happen), then definitely release the bus
//   if (micros() - txTime > DXL_TX_TIMEOUT) {
//     setRX(true);
//     return true;
//   }
//   if (Ser.writeComplete()) {
//     setRX();
//     return true;
//   }
//   // keep waiting
//   return false;
// }

// uint8_t DxlNode::writeByte(uint8_t c) {
//   Ser.write(c);
//   return c;
// }


extern "C" {

void busHandlerTC(void *busObject) {
  DxlNode *dxl = (DxlNode *)busObject;
  digitalWrite(dxl->DE, LOW);
}

void busHandlerRX(void *busObject, uint8_t c) {
  DxlNode *dxl = (DxlNode *)busObject;
  if (dxl->packetReceived == NULL) {
    // this is a master that will do its own processing
    ringBufferStore(c, dxl->Ser.usartMap->rxBuf);
  } else {
    dxl->parseRX(c);
  }

  // // if buffer is empty, then the only acceptable bytes are 0xff
  // // FIXME this is wrong
  // if (dxl->rxIndex <= 1 && byte != 0xff)
  //   return;

  // // store the incoming byte
  // dxl->packet[dxl->rxIndex] = byte;
  // if (dxl->rxIndex > 1)
  //   rxChecksum += byte;
  // if (dxl->rxIndex == 3)
  //   rxLength = byte+4;
  // dxl->rxIndex++;

  // // check if packet is complete
  // if (dxl->rxIndex >= 4) {
  //   // Per protocol, packet[3] = N+2, whereas total bytes = N+6
  //   uint8_t len = dxl->packet[3] + 4;
  //   if (len > DXL_RX_SIZE) {
  //     // packet is invalid? try again from the beginning
  //     dxl->rxIndex = 0;
  //     return;
  //   }

  //   // see if complete
  //   if (dxl->rxIndex == len) {
  //     bool success = false;
  //     uint8_t checksum = 0;
  //     for (int i=2; i<len-1; ++i) {
  //       checksum += dxl->packet[i];
  //     }
  //     checksum = ~checksum;
  //     if (dxl->packet[len-1] != checksum) {
  //       // checksum failed: 
  //       dxl->rxIndex = 0;
  //       return;
  //     }
  //     // checksum passed
  //     if (dxl->isMaster) {
  //       success = true;
  //     } else {
  //       // check address
  //       if (dxl->packet[2] != dxl->myAddress) {
  //         // address didn't match
  //         dxl->rxIndex = 0;
  //         return;
  //       }
  //       else
  //         success = true;
  //     }

  //     // handle a successful packet
  //     if (success) {
  //       // callback
  //       dxl->packetReceived(&dxl->packet[2]);
  //       // dxl->newPacket = true;
  //       dxl->rxIndex = 0;
  //     }
  //   }
  // }

}


}


