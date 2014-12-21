@class Encoder Encoder.h

### Usage

1. (Optionally) call pinRemap() to map the A/B pins to channels 1 and 2 of timer 2, 3, 4, 5, or 19.
2. Call init() to configure the pins and start the encoder
3. (Optionally) call write() at any time to set the count to a desired value
4. Call read() to read the current count

### Example

~~~{.cpp}

#include <Encoder.h>

// Connect encoder signals B and A to PA0 and PA1

Encoder encoder;

void setup() {
  pinRemap(PA0, 2, TIMER5, 1);
  pinRemap(PA1, 2, TIMER5, 2);

  Serial1.begin(115200);

  // Assuming a 12-bit encoder
  encoder.init(PA0, PA1, 4095);
}

void loop() {
  Serial1 << encoder.read() << "\n";
  delay(10);
}

~~~
