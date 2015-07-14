
#include "DxlNode.h"


void DxlNode::init() {
  // Ser.begin(1000000);
  Ser.begin(500000);
  pinMode(DE, OUTPUT);
  digitalWrite(DE, LOW);
}

void DxlNode::sendInstruction(uint8_t id, uint8_t datalength, uint8_t instruction, uint8_t *data) {
  if (!isMaster) return;
  uint8_t checksum = 0;
  setTX();
  writeByte(0xaa);
  // writeByte(0xFF);
  // writeByte(0xFF);
  // checksum += writeByte(id);
  // checksum += writeByte(datalength + 2);
  // checksum += writeByte(instruction);
  // for (uint8_t i=0; i<datalength; ++i) {
  //   checksum += writeByte(data[i]);
  // }
  // writeByte(~checksum);
  setRX();
}

void DxlNode::sendStatus(uint8_t datalength, uint8_t error, uint8_t *data) {
  if (isMaster) return;
  uint8_t checksum = 0;
  setTX();
  // writeByte(0xFF);
  // writeByte(0xFF);
  // checksum += writeByte(myAddress);
  // checksum += writeByte(datalength + 2);
  // checksum += writeByte(error);
  // for (uint8_t i=0; i<datalength; ++i) {
  //   checksum += writeByte(data[i]);
  // }
  // writeByte(~checksum);
  writeByte(0xbb);

  setRX();
}

// both master/slave
bool DxlNode::listenForPacket() {
  // if last two bytes in buffer are 0xff, 0xff, clear the rx buffer

  // uint8_t checksum = 0;

  // USARTxData * s = serialGetStruct(seriali);

  if (Ser.available()) {
    if (isMaster) {
      if (Ser.read() == 0xbb) {
        // sendInstruction(1, 0, 0, 0);
        return true;
        // Serial1 << "RECEIVED\n";
      }
    } else {
      // not sure yet 
      if (Ser.read() == 0xaa) {
        sendStatus(1, 0, 0);
        return true;
        // Serial1 << "RECEIVED\n";
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

