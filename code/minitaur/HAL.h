
#ifndef HAL_h
#define HAL_h

#define USE_BUS 0

#define REMOTE_NUNCHUCK 1
#define REMOTE_RC 2
#define REMOTE_COMPUTER 3

#define REMOTE_TYPE REMOTE_RC

#define IMU_MPU6000 1
#define IMU_VN100 2

#define IMU_TYPE IMU_MPU6000

#define NMOT 8

// MOTORS
#if USE_BUS
#include <DxlMotor.h>
extern DxlMotor M[NMOT];
#else
#include <Motor.h>
extern BlCon34 M[NMOT];
#endif
// #include <OpenLog.h>
#include <BulkSerial.h>
#include <MinitaurLeg.h>

// REMOTE
#if REMOTE_TYPE==REMOTE_NUNCHUCK
#include <Nunchuck.h>
extern Nunchuck nunchuck;
#else
// use hobby transmitter
#define NRECPINS 4
extern volatile float rcCmd[NRECPINS];
#endif

// OTHER PERIPHERALS
extern const uint8_t led0, led1;
extern const int CONTROL_RATE;
extern MinitaurLeg leg[4];
extern float speedDes, yawDes, latDes;
extern BulkSerial openLog;
extern volatile float rolldot, pitchdot;
extern volatile uint32_t totalFailures;

struct LogVector {
  uint16_t align = 0xbbaa;//endianness reverses bytes to aabb
  // regular log
  volatile uint32_t t;//4
  // pitch = body pitch
  volatile float roll, pitch, yaw;//12
  // Motor positions
  volatile float q[8];//44
  // Motor currents
  volatile float torque[8];//76
  // forward velocity
  volatile float xd;//80
  // 8-bit discrete mode
  volatile uint8_t mode;//81
} __attribute__ ((packed));
extern volatile LogVector X;

// struct ComputerPacket {
//   uint8_t cmd1;
//   uint8_t cmd2;
//   float param1;
//   float param2;
// } __attribute__ ((packed));
// extern ComputerPacket computerPacket;

void halInit();
void halUpdate();
void enable(bool);

#endif

