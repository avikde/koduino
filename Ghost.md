@addtogroup Ghost

# Getting started

Page created for http://www.ghostrobotics.io/. This guide assumes the robot is fully assembled.

---

## 1. Getting to know the hardware

### 1.1. Day-to-day usage

#### 1.1.1 Removing the top plate

Unscrew the four bolts on the simple top plate or slide the panel off from between the Picatinny mounts depending on configuration. Remove battery by undoing the velcro strap and try to minimize tension on the battery wires during removal.

![Minitaur with top panel off](../openhatch.jpg "Minitaur with top panel off")

#### 1.1.2. Charging the battery

**WARNING: Handle the battery very carefully.** If the battery is dropped, immediately place it in a LiPo-safe bag and never use it again.

**WARNING: Never over-discharge the battery** Check the voltage every 5-10 minutes while running the robot. If the voltage is lower than 14V, charge the battery.

Use the following charger settings:
1. LiPo Charge 4S
2. Capacity 5300 mAh
3. 10A max current
4. **Never leave a charging battery unattended**

#### 1.1.3. Using the remote

The default mappings as of 2016/07/25:
![Remote mappings in default firmware](../remote.jpg "Remote mappings in default firmware")

**WARNING:** In the default minitaur firmware, the robot starts operating as soon as the remote connects. Make sure the `behavior on/off` switch (top left) is at `2` (down) before turning on the remote.

These mappings, as well as the sensitivities, are not fixed, and can be changed in software. The default firmware comes with a low sensitivity.

#### 1.1.4. Logging data

The default firmware logs
* motor positions,
* motor torque esimates,
* filtered IMU data (angles and angular rates),
* forward speed estimate,
* some discrete states (stance/flight etc.)
at approximately 100 Hz.

To log some data,
1. Get a micro SD card formatted FAT32 (up to 64GB should be fine)
2. Place the following text in a file called `CONFIG.TXT`
~~~
115200,26,3,0,1,1,0
baud,escape,esc#,mode,verb,echo,ignoreRX
~~~
3. Insert the micro SD card in the vertical socket on the mainboard with the gold contacts facing down, and towards the micro USB connector on the mainboard. Be careful, it's easy to insert it crooked, but ultimately it only goes in one way.


To read these logs, first, you need to have Python:
1. Install Python 2.7 using package managers (on Linux/Mac), or from [Anaconda](https://www.continuum.io/downloads) (recommended on Windows)
2. Right click the Windows start button, click `System`, and then click `Advanced system settings` on the left.
3. Click `Environment variables` in the windows that opens. 
4. Click `New...` in the top pane; enter `KODUINO_DIR` for "variable name," and `C:\Users\<username>\Documents\arduino-1.6.1\hardware\koduino\stm32` for "Variable value," where you replace `<username` with your Windows username. Click `OK`.
5. Select the existing environment variable `PATH`, click `Edit...`
6. Make sure these three folders are in the path: `C:\Users\<username>\Anaconda2`, `C:\Users\<username>\Anaconda2\Scripts`, `C:\Users\<username>\Anaconda2\Library\bin`
7. Open a new command prompt or terminal window. Type `conda install numpy scipy matplotlib`
8. To test that that worked, type `python` in command prompt.

To use Python to open the latest log, follow the instructions under the "Example: Reading on a computer" heading in http://avikde.me/koduino/html/class_open_log.html

#### 1.1.5. Modular payload system

![Payload](../payload.jpg "Payload")

### 1.2. Getting familiar with the mainboard

![Mainboard](../mblc0.5.2.jpg "Mainboard")

The two bullet terminals at the bottom are meant to have a high-current hard on/off switch connected across them. We recommend using AWG10 or lower for the wires here. If you are using a power supply to power motors, these can just be connected with a (low AWG) wre.

---

## 2. Programming the mainboard {#programming}

### 2.1. Get your computer ready

The following instructions are for Windows for now.

#### 2.1.1. Get the Arduino IDE

1. Get Arduino 1.6.1 (zip): e.g. [Windows link](http://arduino.cc/download.php?f=/arduino-1.6.1-windows.zip), etc. *Note: it MUST be 1.6.1, and not a later version, for now.*
2. Unzip to a location on your computer (your `Documents\` directory is recommended)

#### 2.1.2. Get the microcontroller libraries

I recommend cloning the repository to your computer so that you can easily update to the latest code in the future.
1. Download [SourceTree](https://www.sourcetreeapp.com/) and install it
2. Open SourceTree, click `Clone / New` on the toolbar
3. Enter the URL `https://github.com/avikde/koduino.git`
4. Under `Destination Path`, click `...` and navigate to
  (a) `Documents\arduino-1.6.1\hardware\koduino` (Windows), or
  (b) `/Applications/Arduino.app/Contents/Resources/Java/hardware` (Mac)
5. Click `Clone` and wait for it to finish
6. If Arduino is open, you must close and re-open it any time you make changes to the `arduino\hardware` directory.

To get the latest version of the libraries at any later time, just open SourceTree, double click `koduino` on the left, and click the `Pull` button on the toolbar.

#### 2.1.3. (Mac only) Get the FTDI drivers

The default Mac FTDI drivers can't do the auto-reset to bootloader.
1. Go to http://www.ftdichip.com/Drivers/VCP.htm and get the latest driver for your OS
2. Restart

### 2.2. Upload a Blink sketch

The main microcontroller board at the center of the robot is called the "mainboard." It's very easy to program it using Arduino.

1. Open `Documents\arduino-1.6.1\arduino.exe`
2. Click `Tools -> Board -> F303V (...)`
3. Select `Tools -> Bootloader -> Serial`, and `Tools -> CPU Speed -> 72 MHz`
4. Make sure the mainboard is *NOT* plugged in
5. Click `Tools -> Port` and look at which COM ports are listed
6. Now plug in a Micro-B USB cable from your computer to the mainboard. Check that at least the red power LED has turned on.
7. Click `Tools -> Port` again; a new COM port should have appeared. Select the new COM port.
8. Paste in the code below
~~~{.cpp}
#include <Arduino.h>

// Change to whatever pin an LED is connected to
const uint8_t led = PD0;

void setup() {
  pinMode(led, OUTPUT);
}

void loop() {
  digitalWrite(led, TOGGLE);
  delay(1000);
}
~~~
9. If you prefer to use the `Makefile` build system (everyone else, specifically those on Windows, please skip this step), you can use the following Makefile
~~~
VARIANT = f303v
UPLOAD_METHOD = SERIAL
UPLOAD_ENTRY = mblc
# UPLOAD_PORT = COM4
HSE_VALUE = 8000000UL
KODUINO_ISRS = INLINE
# 72MHz
CLKSRC = HSE_8M
# overclocked to 112 MHz
# CLKSRC = HSE_8M_OC112

# Don't save EEPROM
EEP_START = 0x0800F000
EEP_LEN = 0
# 256kB
MAX_PROGRAM_SIZE = 262144

LIBRARIES = 

include $(KODUINO_DIR)/project.mk
~~~
10. Click `File -> Upload`, and wait for 10-20 seconds till a success or failure message appears at the bottom. Sometimes if the upload fails, try just unplugging and plugging the mainboard in right after you click "Upload" and trying again. *Hint: you can also enable verbose compilation/upload output in the Arduino console from Arduino's Preferences menu.*
11. After successfully uploading, look at the mainboard. The green LED should be blinking once per second

### 2.3. Programming legs

#### 2.3.1. Leg numbering

![Leg Numbering](../legnumbering.jpg "Leg Numbering")

#### 2.3.2 Zeroing motors

Just once per actuator module, the encoder absolute encoder zeros need to be calibrated.

* The motor zeros are hard-coded for now. To do this, look at the "Setting motor zero" section of the [motor controller library docs](http://avikde.me/koduino/html/group___motor_controller.html).
* [CAD file for leg zeroing jig](../GhostMinitaurLegZeroJig.STL) -- 3D print this to help zero the motors

#### 2.3.3 Driving motors with setOpenLoop and setPosition

See the [motor controller library docs](http://avikde.me/koduino/html/group___motor_controller.html). Specifically, the [MinitaurLeg class](http://avikde.me/koduino/html/class_minitaur_leg.html) will be important.

### 2.4. Template code

Download these (right click, save as)
* [Mainboard template](../code/mblc_template.ino) -- basic mainboard code -- *updated 2016/08/01*
* [Minitaur bounding firmware (BETA)](../code/minitaur.zip) -- *updated 2016/08/01*

**NOTE:** For either of these, **you MUST check the leg zeros** for your specific robot before enabling motors. In the default minitaur firmware, the motors are enabled on power-up!

### 2.5. API documentation

There is a lot of [API documentation](http://avikde.me/koduino/html/)!

---

## 3. FAQ {#faq}

coming soon


