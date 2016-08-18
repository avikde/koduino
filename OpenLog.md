@class OpenLog OpenLog.h

**NOTE: the preferred way to talk to the OpenLog now is using the BulkSerial class. The "reading on a computer" example below is still pertinent and log files are back/forward compatible.**

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

#### Using MATLAB

In a MATLAB command window,
~~~~{.m}
>> cd $KODUINO_DIR/libraries/OpenLog
>> s = open_log('LOG00000.TXT');
Opened LOG00000.TXT
Keys: t,r,p,y,q0,q1,q2,q3,q4,q5,q6,q7,u0,u1,u2,u3,u4,u5,u6,u7,xd,mo
Format: IffffffffffffffffffffB
Finished reading
>> plot(s.t, s.p)

~~~~
Thanks, Pranav Bhounsule, for help with this parsing script.

#### Using Python

First, you need to have Python:
1. Install Python 2.7 using package managers (on Linux/Mac), or from [Anaconda](https://www.continuum.io/downloads) (recommended on Windows)
2. Right click the Windows start button, click `System`, and then click `Advanced system settings` on the left.
3. Click `Environment variables` in the windows that opens. 
4. Click `New...` in the top pane; enter `KODUINO_DIR` for "variable name," and `C:\Users\<username>\Documents\arduino-1.6.1\hardware\koduino\stm32` for "Variable value," where you replace `<username` with your Windows username. Click `OK`.
5. Select the existing environment variable `PATH`, click `Edit...`
6. Make sure these three folders are in the path: `C:\Users\<username>\Anaconda2`, `C:\Users\<username>\Anaconda2\Scripts`, `C:\Users\<username>\Anaconda2\Library\bin`
7. Open a new command prompt or terminal window. Type `conda install numpy scipy matplotlib`
8. To test that that worked, type `python` in command prompt.

Once you have Python, from a terminal,

~~~{.sh}
...$ cd $KODUINO_DIR/libraries/OpenLog
.../OpenLog$ python open_log.py -p <log_file_path_root> [-e <experiment_type>] <log_file_number>
~~~

To display more information about the command line options:
~~~{.sh}
.../OpenLog$ python open_log.py -h
~~~
