# Guide

## Makefile

*NOTE: It seems like the Makefile has errors if there is a space in the username*

* `UPLOAD_METHOD`: Can be `SERIAL` or `DFU`. If the board has an FTDI chip connected to the upload USB port, then pick `SERIAL`, otherwise pick `DFU`.
* `UPLOAD_BAUD` *(optional)*: Baud rate for Serial upload. For wired connections, something like 500000 should work and be quite fast. For wireless bootloading with an XBee, the baud rate must match the XBee's baud rate.
* `LIBRARIES` *(optional)*: Space-separated list of library names. E.g `LIBRARIES = Arduino SPI`. Note that this includes the library directory in the include path, but you still need the `#include <Arduino.h>` (or whatever the header file for the library is called) in the project file.
* The last line of the Makefile must be `include $(STM32DIR)/project.mk`

## Library organization

At the root level, there should be directories called `libraries`, `vendor`, and `wirish`.

### Vendor

This directory contains code that came from the hardware vendor (i.e. library code not written by me). This currently includes the [ARM CMSIS DSP libraries](http://www.doulos.com/knowhow/arm/CMSIS/) which are standard for Cortex-M series ARM microcontrollers, the ST Standard Peripheral Library and the ST CPAL (I2C) library.

These last two libraries are generally not very user friendly, and the only source of documentation are their respective user guides:

* [Standard Peripheral Library](https://bitbucket.org/kodlab/stm32-robot/downloads/stm32_periph.pdf)
* [CPAL](https://bitbucket.org/kodlab/stm32-robot/downloads/stm32_cpal.pdf)

### Wirish

This core library is a thin layer on top of the vendor libraries written purely in C, for no overhead. This includes support for a [system time](../Reference/Time), [timer interrupts](../Reference/Timebase), [external interrupts](../Reference/EXTI), [digital I/O](../Reference/Digital), [analog input](../Reference/ADC), [PWM output and input](../Reference/Timer), etc. Please see the [Reference](../Reference) section for all the functionality exposed by this library.

### Libraries

The `libraries` directory contains libraries in a structure identical to [Arduino's](http://arduino.cc/en/Guide/Libraries). Library code is allowed to be in C or C++.

For example, let us take the `EEPROM` library. It is located in a subdirectory (which must be named the library name) of `libraries`. 
![eeprom_dir_structure.png](https://bitbucket.org/repo/b7yK6p/images/1615822614-eeprom_dir_structure.png)

The source files are located in the first level under the library directory, and an `examples` subdirectory under the library may contain example projects.

Including a library in a project is [easy](QuickStart). 

For most purposes, you will want to include the `Arduino` library in any project (the only exceptions to this would be situations in which you must avoid C++ for overhead reasons, etc.) Adding the Arduino library adds functionality like [Serial](Reference/Serial), [Stream](http://arduino.cc/en/Reference/Stream), [Streaming](http://arduiniana.org/libraries/streaming/), some [math functionality](Reference), a [String class](http://arduino.cc/en/Reference/StringObject), etc.


