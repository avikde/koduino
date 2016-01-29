@addtogroup IMU

### Usage

1. Define the hardware interface as an instance of the IMU abstract base class, e.g. MPU6000
2. Define an attitude filter as an instance of the OrientationFilter abstract base class, e.g. ComplementaryFilter, or EKF
3. In your update loop, call both IMU.readSensors() to poll the sensor, and OrientationFilter.update() to update the filter 
4. Read off filtered euler angles by calling OrientationFilter.getEuler()

### Example: MPU6000

~~~{.cpp}

#include <Arduino.h>

// Uncomment the board that you have
// // Mainboard 1.1
// MPU6000 mpu;
// const uint8_t CS = PC15;
// MBLC
MPU6000 mpu(SPI_2);
const uint8_t CS = PB12;

volatile float roll, pitch;

void controlLoop() {
  mpu.readSensors();
  ekf.update(mpu.acc, mpu.gyr);
  const EulerState *e = ekf.getEuler();
  // The sign and/or order of these need to be edited according to how the chip is oriented on the robot
  roll = e->angles[0];
  pitch = e->angles[1];
}

void setup() {
  Serial1.begin(115200);

  // Wait for MPU6000
  delay(150);
  mpu.init(CS);
  mpu.readSensors();
  ekf.init(mpu.acc);

  attachTimerInterrupt(0, controlLoop, 1000);
}

void loop() {
  Serial1 << roll << "\t" << pitch << "\n";
  delay(10);
}

~~~
