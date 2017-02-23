

The repository associated with this library is at https://github.com/avikde/koduino.

This is a library of code for boards developed using the STM32 F3/F4 series of microcontrollers, chosen for their FPU's and friendly built-in bootloaders.
The microcontrollers currently being used are (see [porting guide](Porting.md) for details) F301, F302, F303, F373, F405, F446.

The API functions here are in general optimized for speed and concurrency, at the expense of being slightly less fool-proof than Arduino (e.g. see [pin configuration](@ref Pins), [PWM output and input](@ref PWM)).

## Quick links

* [Installation & Setup Guide](Installation.md)
* [Porting to new chips](Porting.md)

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
* [Serial with DMA](@ref BulkSerial) --- for when packet sizes are known apriori
* [SPI](@ref SPIClass) --- MPU-6000, ...
* [Wire / I2C / SMBus](@ref TwoWire) --- Nunchuck, MPU-6050, ...
* [Dxl real-time bus (TODO)](@ref Dxl)

### Peripheral device drivers

* [Reading a quadrature encoder](@ref Encoder)
* [Data logging: OpenLog driver](@ref OpenLog) --- on Sparkfun OpenLog, MBLC, or Mainboard v1
* [IMU drivers and orientation filter](@ref IMU) --- MPU6000 drivers, complementary filter, EKF
* [Vectornav IMU driver](@ref VN100)
* [Data logging: FatLog over SDIO (TODO)](@ref FatLog) --- on Mainboard v2 only
* [Wii Nunchuck (TODO)](@ref Nunchuck)

### Libraries

* [Storing data in non-volatile memory (EEPROM)](@ref EEPROMClass)
* [Basic and advanced Math](@ref Math) --- also, the [Eigen](http://eigen.tuxfamily.org/index.php?title=Main_Page) libraries exist
* [Motor controller library with multi-motor coordination](@ref MotorController)
* [Real-time clock (TODO)](@ref RTCClass) --- on Mainboard v2 only
* [Behaviors base class, clocked gaits (TODO)](@ref Behavior)
<!-- * [Brushless commutation with field-oriented control (WIP)](@ref Brushless) --- *Warning: advanced!* -->

