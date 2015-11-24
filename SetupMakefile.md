
# Makefile Setup

### 1. Get the code

* Clone the [repository](https://github.com/avikde/koduino) (using git on the command line, or Sourcetree, etc.)
* Add to `.bash_profile` (Mac) or `.bashrc` (Linux / Cygwin)
~~~
export KODUINO_DIR=<koduino_location>/stm32
~~~

### 2. Toolchain setup

If you have the [Arduino IDE](http://arduino.cc/en/main/software) **1.6.1**

* Add to `.bash_profile` (Mac) or `.bashrc` (Linux / Cygwin)
~~~
export PATH=$PATH:/Applications/Arduino.app/Contents/Resources/Java/hardware/tools/gcc-arm-none-eabi-4.8.3-2014q1/bin
~~~

...otherwise, 

* Download [gcc-arm-embedded 4.8 2014 q1](https://launchpad.net/gcc-arm-embedded/4.8/4.8-2014-q1-update) for your platform
* Add to `.bash_profile` (Mac) or `.bashrc` (Linux / Cygwin)
~~~
export PATH=$PATH:<directory_you_extracted>/bin
~~~

In either case, test that it worked by running
~~~
arm-none-eabi-gcc --version
~~~
from a new terminal window.

### 3. Write your first program

#### Makefile template

In a new directory, create a file called `Makefile` with the following contents.

~~~{.sh}
VARIANT = f37x
HSE_VALUE = 16000000UL
UPLOAD_METHOD = SERIAL
UPLOAD_BAUD = 230400
# Put libraries you want to use here
LIBRARIES = SPI Wire

include $(KODUINO_DIR)/project.mk
~~~

#### Sketch template

Create another file called `Blink.cpp` with the following contents. (This is the Blink example from the [Digital output](@ref Digital) section.)

~~~{.cpp}

#include <Arduino.h>

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

### 4. Upload the code

Run `make` in this directory.  *Hint: Sublime Text 3 with the "Make" build system is very convenient for this.*


<!-- 
* Run `brew install dfu-util` (to use the MAEVARM M4 and some older boards).
* Install the latest [FTDI drivers](http://www.ftdichip.com/Drivers/VCP.htm).
* Run `pip install pyserial progressbar`.
* Clone this repository (ask for invite) using `hg clone` followed the URL in the top right of this webpage (use HTTPS if unsure).
* Add `export KODUINO_DIR=<koduino>/` to your `~/.bash_profile`.

 -->