
# Arduino IDE Setup

### 1. Get the IDE

* Get Arduino 1.6.1: [Windows](http://arduino.cc/download.php?f=/arduino-1.6.1-windows.zip), etc.

### 2. Get the code

* Create a directory called `koduino` under `Arduino\hardware` (next to `arduino`)
* Clone the [repository](https://github.com/avikde/koduino) using Sourcetree (or whatever) into the `koduino` directory.
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