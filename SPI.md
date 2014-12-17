@class SPIClass SPI.h

### Usage

1. (Optionally) call setPins() (see its documentation for default pins)
2. Call begin()
3. (Optionally) call setClockDivider() (default `SPI_CLOCK_DIV2`)
4. (Optionally) call setBitOrder() (default `MSBFIRST`)
5. (Optionally) call setDataMode() (default `SPI_MODE0`)
6. Before starting any transfer the chip select pin must be driven low by the master
7. To write to the slave, call transfer() with the byte to write
8. To read from the slave, call transfer() with `0` and record the returned value
9. Drive chip select high to end the transmission

### Example: MPU-6000

~~~{.cpp}
#include <SPI.h>

// Connect to the chip select pin of the device
const int csPin = PC15;
// See setPins() for SCK, MISO, MOSI defaults

void setup() {
  Serial1.begin(115200);
  
  SPI.setPins(PA12, 6, PA13, 6, PF6, 5);
  pinMode(csPin, OUTPUT);
  digitalWrite(csPin, HIGH);
  
  SPI.begin();
  SPI.setClockDivider(SPI_CLOCK_DIV2);
  SPI.setBitOrder(MSBFIRST);
  SPI.setDataMode(SPI_MODE0);
}

void loop() {
  uint32_t tic = micros();
  digitalWrite(csPin, LOW);
  delayMicroseconds(1);
  SPI.transfer(0x75 | 0x80);
  uint8_t magic = SPI.transfer(0) & 0x7E;
  digitalWrite(csPin, HIGH);
  tic = micros() - tic;
  
  Serial1 << "WHO_AM_I = 0x" << _HEX(magic) << ", took " << tic << "us.\n";

  delay(100);
}
~~~
