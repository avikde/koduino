@class Stream Stream.h

## Stream, Print, String

Stream is a collection of functions for ASCII or binary streams used by [Serial](@ref USARTClass), [Wire](@ref TwoWire), etc. These methods are identical to the [Arduino equivalents](http://arduino.cc/en/Reference/Stream).

The Arduino [String library](http://arduino.cc/en/Reference/StringObject) is included as well.

## Streaming

The excellent [Streaming library](http://arduiniana.org/libraries/streaming/) is also available.

### Example

~~~{.cpp}

#include <Arduino.h>

void setup() {
  Serial1.begin(115200);
}

void loop() {
  Serial1 << "The time is " << millis() << " and PI = " << PI << "\n";
}

~~~

