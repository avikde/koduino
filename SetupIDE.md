
# Arduino IDE Setup

### 1. Get the IDE

* Get **1.6.1** version of the [Arduino IDE](http://arduino.cc/en/Main/Software) (you may have to click "Previous Releases"), and install it.

### 2. Get the code

* Clone the [repository](https://github.com/avikde/koduino), or get a [zipball of the code](http://github.com/avikde/koduino/zipball/master/), unzip it, and rename to `koduino`.
* Quit the Arduino IDE if it is running.
* Move the koduino code into the Arduino IDE `hardware` directory (`C:\Program Files (x86)\Arduino\hardware` on Windows, `/Applications/Arduino.app/Contents/Java/hardware` on Mac).
* The file/folder structure should match
![IDE file structure](../ide_file_structure.png "IDE file structure")

### 3. Write and upload your first program

* Create a file called `Blink.ino` with the following contents. (This is the Blink example from the [Digital output](@ref Digital) section.)

~~~{.cpp}

// Change to whatever pin an LED is connected to
const int led = PC13;

void setup() {
  pinMode(led, OUTPUT);
}

void loop() {
  digitalWrite(led, TOGGLE);
  delay(1000);
}

~~~

* Double-click the file to open it in the Arduino IDE.
* Select `Tools -> Board -> f37x`.
* Plug in the boad and select the (hopefully only) option in `Tools -> Port` (that isn't `Bluetooth-Modem` or `Bluetooth-Incoming-Port`). *On Mac OS, select the "/dev/tty." option*.
* Hit `Cmd+u` (Mac) or `Ctrl+u` to compile and upload. *If it says "Failed to init" at the bottom, please try again.*