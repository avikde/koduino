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



void DxlNode::init() {
  Ser.begin(2500000);
  digitalWrite(DE, LOW);
  pinMode(DE, OUTPUT);
}

void DxlNode::sendPacket(uint8_t id, uint8_t instErr, uint8_t N, uint8_t *params, bool wait) {
  // if (!isMaster) return;
  uint8_t checksum = 0;
  setTX();
  // writeByte(0xaa);
  writeByte(0xff);
  writeByte(0xff);
  checksum += writeByte(id);
  checksum += writeByte(N + 2);
  checksum += writeByte(instErr);
  for (uint8_t i=0; i<N; ++i) {
    checksum += writeByte(params[i]);
  }
  writeByte(~checksum);
  if (wait)
    setRX();
}

bool DxlNode::checkPacket() {
  // check checksum
  uint8_t checksum = 0;
  uint8_t len = packet[3];
  for (int i=2; i<len+3; ++i) {
    checksum += packet[i];
  }
  checksum = ~checksum;
  if (packet[len+3] != checksum)
    return false;

  // good packet received
  if (isMaster) {
    return true;
  } else {
    // slave should only respond if addressed
    return (packet[2] == myAddress);
  }

}

// both master/slave
bool DxlNode::listen() {
  // the smallest packet size
  if (Ser.available() >= 4) {
    // packet might be aligned with start of buffer
    if (Ser.peekAt(0) == 0xff && Ser.peekAt(1) == 0xff) {
      // packet[3] = N+2, whereas total bytes = N+6
      uint8_t len = Ser.peekAt(3) + 4;
      if (len > DXL_MAX_PACKET_SIZE) {
        // packet is invalid? pop off the top byte and try again
        Ser.read();
        return false;
      }

      if (Ser.available() >= len) {
        // read this packet
        for (int i=0; i<len; ++i) {
          packet[i] = Ser.read();
        }
        // check this packet
        return checkPacket();
      }
    } else {
      // there are bytes in the buffer but not 0xffff
      // get rid of the first byte and try again
      Ser.read();
    }
  }
  return false;
}

void DxlNode::setTX() {
  digitalWrite(DE, HIGH);
  delayMicroseconds(10);
}

void DxlNode::setRX() {
  Ser.flush();
  delayMicroseconds(10);
  digitalWrite(DE, LOW);
  // delayMicroseconds(5);
}

bool DxlNode::completeTX() {
  if (Ser.writeComplete()) {
    digitalWrite(DE, LOW);
    return true;
  }
  return false;
}

uint8_t DxlNode::writeByte(uint8_t c) {
  Ser.write(c);
  return c;
}

