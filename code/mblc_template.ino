
#define IMU_MPU6000 1
#define IMU_VN100 2
#define IMU_TYPE IMU_VN100

#include <Arduino.h>
#include <BulkSerial.h>
#include <Motor.h>
#include <SPI.h>
#if IMU_TYPE==IMU_MPU6000
#include <MPU6000.h>
#include <EKF.h>
#include <Eigen.h>
#elif IMU_TYPE==IMU_VN100
#include <VN100.h>
#endif

const int CONTROL_RATE = 1000;

// Pins
const uint8_t led1 = PD0, led2 = PD1;
const uint8_t pwmPin[] = {PE9, PE11, PE13, PE14, PA0, PD4, PD7, PD6, PB4, PB5};
const uint8_t posPin[] = {PD12, PD13, PD14, PD15, PC6, PC7, PC8, PC9, PE2, PE3};
const uint8_t curPin[] = {PD8, PB2, PA13, PB1, PB0, PD5, PF9, PF10, PE4, PE5};
// first 6 channels of curPin[] are PWM_IN_EXTI, not PWM_IN

// RC receiver
const uint8_t rcRecPin[] = {PC15, PC14, PC13, PB7, PB6, PD3};
volatile float rcCmd[6];

// Motors
const int NMOT = 10;
const uint8_t motorPort[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
BlCon34 M[NMOT];
volatile float currents[10];
const float motZeros[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
const int8_t dir[] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1};

// Logging
BulkSerial openLog(MBLC_OPENLOG);
struct StateVector {
  uint16_t align = 0xbbaa;//endianness reverses bytes to aabb
  uint32_t t;
  float x;
  uint8_t bit;
} __attribute__ ((packed));
StateVector state;

// IMU
#if IMU_TYPE==IMU_MPU6000
// IMU
MPU6000 mpu(SPI_2);
EKF ekf(0.1, 1.0, 1/((float)CONTROL_RATE));
#elif IMU_TYPE==IMU_VN100
VN100 vn100(SPI_2);
#endif
float roll, pitch, yaw;

void controlLoop() {
  for (int i=0; i<NMOT; ++i) {
    M[i].update();
    // currents[i] = pwmIn(curPin[motorPort[i]]);
  }

#if IMU_TYPE==IMU_MPU6000
  mpu.readSensors();
  ekf.update(mpu.acc, mpu.gyr);
  const EulerState *e = ekf.getEuler();
  // put in IMU offsets here
  // convention: roll right > 0, pitch forward > 0
  roll = e->angles[0];
  pitch = e->angles[1];
  yaw = e->angles[2];
  // rolldot = e->angRates[0];
#elif IMU_TYPE==IMU_VN100
  float _yawdot=0, _rolldot=0, _pitchdot=0;
  vn100.get(yaw, roll, pitch, _yawdot, _rolldot, _pitchdot);
#endif

  // RC receiver
  for (int i=0; i<6; ++i) {
    int period=0, pulsewidth=0;
    pwmInRaw(rcRecPin[i], &period, &pulsewidth);
    rcCmd[i] = pulsewidth*100/((float)period);
  }

  // CONTROL
  // for (int i=0; i<NMOT; ++i) {
  //   M[i].setGain(0.5);
  //   M[i].setPosition(arm_sin_f32(millis() * 0.001 * TWO_PI));
  //   // M[i].setOpenLoop(0.1);
  // }
}

void debug() {
  digitalWrite(led1, TOGGLE);

  // IMU
  Serial1 << millis() << "\t";
  Serial1 << roll << "\t" << pitch << "\t" << yaw << "\t";

  // Print positions
  // for (int i=0; i<NMOT; ++i) {
  //   Serial1.print(M[i].getRawPosition(), 2);
  //   Serial1 << "\t";
  // }

  // CURRENTS
  // for (int i=0; i<NMOT; ++i)
  //   Serial1 << currents[i] << "\t";

  // // RCREC
  // for (int i=0; i<6; ++i)
  //   Serial1 << rcCmd[i] << "\t";

  // NUNCHUCK
  // Serial1 << nunchuck.isPaired;

  // TEST log
  state.t = millis();
  state.x = PI;
  state.bit = 1;
  openLog.write();

  // while (USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
  // USART_SendData(USART3, 'a');

  Serial1 << "\n";
}

void setup() {
  Serial1.begin(115200);
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);

  // Motors
  Motor::updateRate = CONTROL_RATE;
  Motor::velSmooth = 0.55;
  for (int i=0; i<NMOT; ++i) {
    uint8_t port = motorPort[i];
    M[i].init(pwmPin[port], posPin[port], motZeros[i], dir[i]);
  }

#if IMU_TYPE==IMU_MPU6000
  // delay for IMU
  delay(150);
  mpu.init(PB12);// returns false on failure
  // Start off the EKF
  mpu.readSensors();
  ekf.init(mpu.acc);
#elif IMU_TYPE==IMU_VN100
  vn100.init(PB12);
#endif

  // RC receiver
  for (int i=0; i<6; ++i)
    pinMode(rcRecPin[i], PWM_IN_EXTI);

  // Raspi port: 
  // Serial2.begin(115200);
  
  // Try to start logger
  openLog.begin(115200, sizeof(state), &state, 0, NULL);
  openLog.initOpenLog("t,pi,bit", "IfB");
  openLog.enable(true);
  
  // 
  attachTimerInterrupt(0, controlLoop, 1000);
  attachTimerInterrupt(1, debug, 20);

  digitalWrite(led1, HIGH);
  digitalWrite(led2, HIGH);

  // TESTING
  for (int i=0; i<NMOT; ++i) {
    M[i].setOpenLoop(0.1);
    // M[i].setGain(0.6);
    // M[i].setPosition(2.75);
  }
  // delay(1000);
  for (int i=0; i<NMOT; ++i) {
    M[i].enable(true);
  }
}

void loop() {
}