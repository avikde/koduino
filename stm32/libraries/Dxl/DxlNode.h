#ifndef DxlNode_h
#define DxlNode_h

#include <Arduino.h>

class DxlNode {
public:
  // members
  const uint8_t DE, myAddress;
  USARTClass& Ser;
  const bool isMaster;
  
  // incoming packet
  uint8_t buf[32];

  // functions
  DxlNode(uint8_t rts, USARTClass& ser, uint8_t myAddress) : DE(rts), myAddress(myAddress), Ser(ser), isMaster(false) {}
  DxlNode(uint8_t rts, USARTClass& ser) : DE(rts), myAddress(0), Ser(ser), isMaster(true) {}


  void init();
  void sendInstruction(uint8_t id, uint8_t datalength, uint8_t instruction, uint8_t *data);
  void sendStatus(uint8_t datalength, uint8_t error, uint8_t *data);
  // both master/slave
  bool listenForPacket();
  bool checkPacket();


protected:
  void setTX();
  void setRX();
  uint8_t writeByte(uint8_t c);
};


#endif
