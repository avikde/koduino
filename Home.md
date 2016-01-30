

The repository associated with this library is at https://github.com/avikde/koduino.

This is a library of code for boards developed using the STM32 F3/F4 series of microcontrollers, chosen for their FPU's and friendly built-in bootloaders.
The microcontrollers currently targeted are:

* [F301](http://www.st.com/stm32f301-pr)
* [F302](http://www.st.com/stm32f302-pr1)
* [F373](http://www.st.com/web/catalog/mmc/FM141/SC1169/SS1576/LN10)
* [F405](http://www.st.com/web/en/catalog/mmc/FM141/SC1169/SS1577/LN1035)

The API functions here are in general optimized for speed and concurrency, at the expense of being slightly less fool-proof than Arduino (e.g. see [pin configuration](@ref Pins), [PWM output and input](@ref PWM)).

## Quick links

* [Installation & Setup Guide](Installation.md)
* [Porting to new chips](Porting.md) - **Not completed yet**

<!-- 
* [Writing your first program](Guide.md)
* [Uploading your code](Bootloading.md)
 -->

## API Reference

### Core functionality

* [Pin configuration](@ref Pins) --- pinMode, ...
* [Digital I/O](@ref Digital) --- digitalWrite, digitalRead, ...
* [Reading analog signals](@ref Analog) --- analogRead, ...
* [PWM output and input](@ref PWM) --- analogWrite, pulseIn, ...
* [Keeping time](@ref Time) --- millis, micros ...
* [Running code with precise timing](@ref Timebase) --- attachTimerInterrupt, ...
* [External (or pin-change) interrupts](@ref EXTI) --- attachInterrupt, ...
* [Printing, streaming and strings](@ref Stream) --- Serial1 <<, ...

### Communication

* [Serial / USART](@ref USARTClass) --- FTDI (USB/Serial), XBee, ...
* [SPI](@ref SPIClass) --- MPU-6000, ...
* [Wire / I2C / SMBus](@ref TwoWire) --- Nunchuck, MPU-6050, ...
* [Real-time RS485-based bus (TODO)](@ref Dxl)

### Basic libraries

* [Storing data in non-volatile memory (EEPROM)](@ref EEPROMClass)
* [Reading a quadrature encoder](@ref Encoder)
* [Real-time clock (TODO)](@ref RTCClass)
* [Data logging: OpenLog driver](@ref OpenLog)
* [Data logging: FatLog over SDIO (TODO)](@ref FatLog)

### Robotics-oriented libraries

* [Basic and advanced Math](@ref Math) --- also, the [Eigen](http://eigen.tuxfamily.org/index.php?title=Main_Page) libraries exist
* [Motor controller library with multi-motor coordination](@ref MotorController)
* [IMU drivers and orientation filter](@ref IMU) --- MPU6000 drivers, complementary filter, EKF
* [Brushless commutation with field-oriented control (WIP)](@ref Brushless) --- *Warning: advanced!*

