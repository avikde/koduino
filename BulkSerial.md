@class BulkSerial BulkSerial.h

### Usage

TODO!


### Example (on MBLC 0.5 to talk to the OpenLog)

~~~
#include <BulkSerial.h>

// Logging
BulkSerial openLog(MBLC_OPENLOG);
struct StateVector {
  uint16_t align = 0xbbaa;//endianness reverses bytes to aabb
  uint32_t t;
  float x;
  uint8_t bit;
} __attribute__ ((packed));
StateVector state;

void setup() {
  openLog.begin(115200, sizeof(state), &state, 0);
  openLog.initOpenLog("t,pi,bit", "IfB");
  openLog.enable(true);
}

void loop() {
  state.t = millis();
  state.x = PI;
  state.bit = 1;
  openLog.write();
  delay(10);
}
~~~

### Example (on MBLC 0.5 to talk to the raspberry pi serial port)

~~~
#include <BulkSerial.h>

BulkSerial rpi(MBLC_RPI);

uint8_t txBuf[10]= {0xaa, 0xbb, 1, 2, 3, 4, 5, 6, 7, 8};
uint8_t rxBuf[10] = {0,0,0,0,0,0,0,0,0,0};

volatile uint32_t lastWrite = 0, lastRx = 0;

void controlLoop() {
  uint32_t t = millis();
  // this will print when it receives a full packet, aligned with {0xaa,0xbb}
  int res = rpi.received(0xbbaa, rxBuf);
  if (res) {
    lastRx = t;
    Serial1 << t << "\t";
    for (int i=0; i<10; ++i) {
      Serial1 << rxBuf[i] << "\t";
    }
    Serial1 << "\n";
  } 

  if (t - lastWrite > 9) {
    digitalWrite(PD0, TOGGLE);
    rpi.write();//send the txBuf
    lastWrite = t;
  }
}

void setup() {
  pinMode(PD0, OUTPUT);
  Serial1.begin(115200);
  rpi.begin(115200, 10, txBuf, 10);
  rpi.enable(true);// this enables the transmit

  attachTimerInterrupt(0, controlLoop, 1000);
}

void loop() {
}
~~~