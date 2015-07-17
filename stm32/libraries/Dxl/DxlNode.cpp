
#include "DxlNode.h"


void DxlNode::init() {
  Ser.begin(2500000);
  pinMode(DE, OUTPUT);
  digitalWrite(DE, LOW);
}

void DxlNode::sendPacket(uint8_t id, uint8_t instErr, uint8_t N, uint8_t *params) {
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
    if (Ser.peekAt(0) == 0xff && Ser.peekAt(1) == 0xff) {
      // packet[3] = N+2, whereas total bytes = N+6
      uint8_t len = Ser.peekAt(3) + 4;
      if (Ser.available() >= len) {
        // read this packet
        for (int i=0; i<len; ++i) {
          packet[i] = Ser.read();
        }
        // check this packet
        return checkPacket();
      }
    }
  }
  return false;
}

void DxlNode::setTX() {
  digitalWrite(DE, HIGH);
  // delayMicroseconds(10);
  // delay(1);
}

void DxlNode::setRX() {
  Ser.flush();
  // delayMicroseconds(10);
  // delay(1);
  digitalWrite(DE, LOW);
}

uint8_t DxlNode::writeByte(uint8_t c) {
  Ser.write(c);
  return c;
}

