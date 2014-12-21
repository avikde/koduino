@class OpenLog OpenLog.h

This is a library of high level functions for [OpenLog v3 firmware](https://github.com/sparkfun/OpenLog/tree/master/firmware/OpenLog_v3) attached on some serial port. A lot of the functions are from the [read example from SparkFun](https://github.com/sparkfun/OpenLog/blob/master/firmware/examples/OpenLog_ReadExample/OpenLog_ReadExample.ino). All the file ops are performed by OpenLog, and this library adds

* a hardware interface including resetting, opening and verifying a write connection, 
* "as fast as possible" (only limited by Serial write speed) binary dumping with an alignment word, and
* a Python API for reading the binary format and plotting the data.

### Usage

1. Set up the hardware connections with the constructor OpenLog().
2. Declare a packet structure with `__attribute__ ((packed))`.
3. Call init() with the header, format and size of the declared packet structure.
4. Populate the structure fields while the program runs.
5. Call enable() with `true` to start logging, `false` to stop.
6. Call write() as often as desired (it will only write after the previous write finishes), or from a [timer interrupt](@ref Timebase).

### Example: Writing from the microcontroller (Mainboard v1.1)

~~~{.cpp}

#include <Arduino.h>

// Declare a custom packet structure
struct StateVector {
  uint32_t t;
  float PI;
  uint8_t bit;
} __attribute__ ((packed));
StateVector state;
OpenLog openLog(PB2, Serial2, 115200);

void setup() {
  // Make sure the header and format match the struct definition above
  openLog.init("t,PI,bit", "IfB", sizeof(state));
}

void loop() {
  // Log for 5 seconds
  openLog.enable(millis() < 5000);
  state.t = millis();
  state.PI = PI;
  state.bit = 1;
  openLog.write((const uint8_t *)&state);
}

~~~

### Example: Reading on a computer

From a terminal,

~~~{.sh}
...$ cd $KODUINO_DIR/libraries/OpenLog
.../OpenLog$ python open_log.py -p <log_file_path_root> [-e <experiment_type>] <log_file_number>
~~~

To display more information about the command line options:
~~~{.sh}
.../OpenLog$ python open_log.py -h
~~~
