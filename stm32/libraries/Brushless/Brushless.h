/**
 * @authors Avik De <avikde@gmail.com>

  This file is part of koduino <https://github.com/avikde/koduino>

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation, either
  version 3 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, see <http://www.gnu.org/licenses/>.
 */
#ifndef Brushless_h
#define Brushless_h

#include <Arduino.h>

enum CommutationType {
  BLOCK, SINUSOIDAL, TRAPEZOIDAL
};

enum ControlMode {
  VOLTAGE_CONTROL, POSITION_CONTROL, CURRENT_CONTROL, DEBUGGING, DEBUGGING_SIN
};

// TO use this class:
// 1. define a setOutputs function globally
void setOutputs(bool e1, float p1, bool e2, float p2, bool e3, float p3);
// 2. define posRead, posWrite functions globally that read and write rotor position very quickly. generalizations of encoder.read(), encoder.write()
uint16_t posRead();
void posWrite(uint16_t pos);

class Brushless {
public:
  volatile float amplitude = 0, prevAmplitude = 0;
  volatile float pos_act_01 = 0;
  volatile int commutationTime = 0;
  volatile float elang;
  volatile uint16_t currents[3];
  volatile int16_t current; // scalar combining phase currents
  volatile float posRad;

  /**
   * @brief Command the phases for a specific electrical angle
   * @param electricalAngle Current electrical angle in [0, 1]
   * @param amplitude Amplitude in [-1, 1]
   * @param waveform One of `BLOCK`, `SINUSOIDAL`, `TRAPEZOIDAL`
   */
  void setMotorPhases(float electricalAngle, float amplitude, CommutationType waveform);
  /**
   * @brief Set basic motor properties (call before init())
   * @param polePairs Number of pole pairs
   * @param leadFactor Lead factor (start with 0 and hand-tune)
   */
  void setMotorProps(int polePairs, float leadFactor);

  /**
   * @brief Initialize commutation from absolute position
   * @details Call this right before calling attachTimerInterrupt() with the commutation function
   * 
   * @param absPos Absolute rotor position
   * @param commutationRate Rate at which commutation will occur
   */
  void init(uint32_t absPos, int commutationRate);

  /**
   * @brief This function reads the position and sets the motor phases; should be called at at least 5KHz
   */
  void commutate();
  int16_t readCurrent();

  /**
   * @brief Calibration routine which (a) detects wire orientation, and (b) determines magnet zero by spinning back and forth
   * @details This will first command 3 positions in open loop to determine the order of the wires, and write this setting to EEPROM position 1. Next, it will spin the motor back and forth, doing gradient descent on the total angle swept till it spins equally well in both directions. It writes the magnet zero to EEPROM position 0.
   * 
   * @param sweepAmplitude The amplitude of the sweeping back and forth. This should be increased for low Kv motors, decreased when bus voltage is higher.
   * @param convergenceThreshold Threshold of difference between left and right sweeps (default is 1)
   */
  void calibrate(float sweepAmplitude, float convergenceThreshold);
  void calibrate(float sweepAmplitude) {calibrate(sweepAmplitude, 1);}

  /**
   * @brief Update the desired amplitude
   * @details This does not need to be called as frequently as commutation, or at a regular rate (for example, this can go in loop()).
   * 
   * @param pwmInput The PWM input. Acceptable duty cycle is [0.1, 0.9], which is mapped to [-1, 1] motor amplitude. Anything outside [0.1, 0.9] disables the motor as a communication failsafe.
   */
  void update(float pwmInput);

  void setEncoderCPR(uint16_t cpr) {encoderCPR = cpr;}
  void setDebuggingAmplitude(float a) {debuggingAmplitude = a;}
  void setMode(ControlMode theMode) {mode = theMode;}
  void setWaveform(CommutationType wave) {waveform = wave;}
  void setSpeedLimit(float vlim) {speedLimit = vlim;}
  void setAmplitudeLimit(float alim) {ampLim = alim;}

  float getPosition() { return posRad; }
  float getVelocity() { return motorVel; }

  void openLoopTest(uint32_t pause, float amplitude);
  void openLoopTest(uint32_t pause) {openLoopTest(pause, 1);}

  // Modified from Piccoli
  void calcSpaceVector(float electricalAngle, float amplitude, uint8_t *enables, float *pwms, CommutationType commutation);

  // Helper
  float posToRadians(uint32_t pos);
  // This should be in EEPROM
  // TODO: write a initialization part that uses Serial to ask for this and any other motor-specific parameters?
  int pos_zer=0;
// protected:
  
  DLPF velF, accF;
  int POLE_PAIRS;
  float leadFactor = 0;
  float posctrl0 = 0, motorVel = 0, motorAcc = 0, posDes = 0;
  float velInt = 0;
  bool motorEnableFlag = false;
  float countsPerElecRev = 0; // calculated
  volatile uint8_t section = 0;
  volatile int pos_act;
  float debuggingAmplitude;
  ControlMode mode;
  CommutationType waveform = SINUSOIDAL;
  uint16_t encoderCPR = 4096;
  float speedLimit = 0;
  bool flipWires = false;
  float ampLim = 1;
};


#endif
