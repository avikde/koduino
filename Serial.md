@class USARTClass USARTClass.h

### Usage

1. Call begin() with a baud rate (and optionally the mode)
2. Use any of the [Stream](@ref Stream) output methods to read or write

### Example: Hello

~~~{.cpp}


void setup() {
  Serial1.begin(115200);
}

void loop() {
  Serial1 << "Hello\n";
  delay(1000);
}

~~~
