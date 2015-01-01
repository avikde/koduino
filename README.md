Koduino
=======

This is a library of code for boards developed using the STM32 F3/F4 microcontrollers, chosen for their great price/performance ratio, friendly built-in bootloaders, and (for the Cortex-M4F ones) FPU support.

### Supported hardware

* **Upload methods:** Serial (incl using USB-Serial adapters), USB using DFU (soon)
* **Chip families:** F37x, F4xx (soon), F1xx (planned)
* **Chips tested:** F373CC, F405RG (soon), F1?? (planned)

### Supported software

* 95% of the Arduino core (felt no urgency to implement `shiftOut` or `tone` but they should be easy)
* Libraries: `Wire`, `SPI`, `EEPROM`, `Encoder`, etc. Important ommisions: `Serial` (using USB CDC) is not supported yet, but should be soon.
* [Documentation!](http://avikde.me/koduino/html/)

### Why does this library exist?

Roger Clark compiled a [list of other STM32 Arduino compatibility projects](https://github.com/rogerclarkmelbourne/Arduino_STM32/wiki/Other-STM32-Arduino-projects). I really began working on this because my initial F373-based hardware was not supported by any of them. Even so, I think this library may have some of the folowing benefits for others as well.

#### As a user

1. Works with the Arduino IDE
1. Hands-free programming on boards with hardware support (FTDI chip with NRST/BOOT0 connections)
1. Wireless programming on boards with hardware support (wireless-UART like XBee)
1. Already in use in my several of my own critical projects, including
    1. "Flight control" boards with EKF orientation filtering, motor control and data logging @ 2 KHz
    1. Field-oriented brushless commutation @ 25 KHz

#### As a developer

1. **Porting to new chips:** This process is much easier than with libmaple, or Aeroquad32, etc. because this library uses a (relatively) portable interface layer, the ST standard periperal library, to try and use the same Arduino core code to interface with several microcontrollers. The alternative, to begin with the register map for each new chip, is tedious at best (trust me, I tried ;) ).
1. **Open-source hardware:** No custom bootloaders are required. You can literally make your own board from one of the reference designs (repo coming soon), or even begin straight from the microcontroller datasheet reference schematic, and get your own hardware working quickly.

### Get started

#### 1. Get code from this repository

If you're used to mercurial, git should be no problem. You can

* Download the repository as a [zip file](https://github.com/avikde/koduino/archive/master.zip), or
* Clone it by typing `git clone https://github.com/avikde/koduino.git` at the location you want.

#### 2. Program microcontrollers

Links to get started are at http://avikde.me/koduino.

### Contribute

If this library is helpful to you, please consider contributing to it. The main areas that need help are:

* Adding support for more chips
* Maintaining upload tools for several OS'es / platforms, debugging wireless uploads, making uploads faster, etc.
* Contributing to the Arduino core: I often leave out functionality that I don't need (e.g. `tone`) but someone might!

You are welcome to fork, and please submit pull requests if you do! Additionally, I would love to have direct collaborators who have commit access---don't hesitate to get it touch.
