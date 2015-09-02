
#include "Brushless.h"
#include <EEPROM.h>

#define PI_BY_3     1.047197551196598f



void Brushless::commutate() {
  static uint32_t tic;
  tic = micros();

  // Get position
  pos_act = encoderCPR - encoder.read();
  posRad = posToRadians(pos_act);
  motorVel = velF.update(posRad);
  velInt += motorVel; // Used by calibration routine
  pos_act_01 = pos_act/((float)encoderCPR);
  // Get electrical angle
  int pos_mod=(pos_act-pos_zer) % countsPerElecRev;//map to a single electrical revolution
  if (pos_mod < 0)
    pos_mod += countsPerElecRev;
  elang = pos_mod / ((float)countsPerElecRev);
  
  // Inductance lag compensation
  elang += leadFactor * motorVel;
  if (elang < 0)
    elang += 1;
  if (elang > 1)
    elang -= 1;

  setMotorPhases(elang, amplitude, waveform);

// #if defined(SERIES_STM32F4xx)
//   // Current sense
//   current = readCurrent();
// #endif

  commutationTime = (1000 + micros() - tic) % 1000;
}

// Block takes <1 us, sinusoidal and trapezoidal take 5 us
void Brushless::calcSpaceVector(float electricalAngle, float amplitude, uint8_t *enables, float *pwms, CommutationType commutation) {
  static const float refVectors[6][3] = {{0,1,1},{0,0,1},{1,0,1},{1,0,0},{1,1,0},{0,1,0}};

  float theta_da, theta_db;
  float t1, t2, t0;                     // Time slots for 3 vectors.
  uint8_t nextSection;
  section = (uint8_t) (electricalAngle * 6.0);

  // Constrain so it makes sense
  amplitude = constrain(amplitude, -1, 1);

  if (commutation == BLOCK)
  {
    // Block goes backwards FIXME??
    amplitude = -amplitude;

    float pwmHigh = 0.5 + 0.5 * amplitude;
    float pwmLow = 0.5 - 0.5 * amplitude;

    // initialize
    enables[0] = enables[1] = enables[2] = 1;
    pwms[0] = pwms[1] = pwms[2] = 0.5;
    switch (section)
    {
      case 0:
        pwms[0] = pwmLow;
        enables[1] = 0;
        pwms[2] = pwmHigh;
        break;
      case 1:
        enables[0] = 0;
        pwms[1] = pwmLow;
        pwms[2] = pwmHigh;
        break;
      case 2:
        pwms[0] = pwmHigh;
        pwms[1] = pwmLow;
        enables[2] = 0;
        break;
      case 3:
        pwms[0] = pwmHigh;
        enables[1] = 0;
        pwms[2] = pwmLow;
        break;
      case 4:
        enables[0] = 0;
        pwms[1] = pwmHigh;
        pwms[2] = pwmLow;
        break;
      case 5:
        pwms[0] = pwmLow;
        pwms[1] = pwmHigh;
        enables[2] = 0;
        break;
    }
  } else {
    theta_da = electricalAngle * TWO_PI - ((float)section)*PI_BY_3;  //Theta, the electrical angle bewteen current angle and section boundary, from 0 to pi/3 
    
    switch (commutation)
    {
      case TRAPEZOIDAL:
        theta_db = arm_sin_f32(theta_da)/arm_sin_f32(PI_BY_3+theta_da); // Scale factor (in simplest form)
        t1 = theta_db*amplitude;            // Time for using vector 1
        t2 = (1-theta_db)*amplitude;        // Time for using vector 2
        t0 = (1-amplitude)*0.5;               // Time for 0 vector.
        break;
      case SINUSOIDAL:
      // sinusoidal
        // Maximum possible torque assumed
        theta_db = PI_BY_3 - theta_da;                   //pi/3-theta.
        t1 = arm_sin_f32(theta_da)*amplitude;
        t2 = arm_sin_f32(theta_db)*amplitude;
        t0 = (1-t1-t2)*0.5;             // Time for 0 vector.
        break;
      default:
        t1 = 0;
        t2 = 0;
        t0 = 0.5;
    }
    nextSection = (section+1)%6;
    pwms[0] = (t0 + t1*refVectors[nextSection][0]) + t2*refVectors[section][0];
    pwms[1] = (t0 + t1*refVectors[nextSection][1]) + t2*refVectors[section][1];
    pwms[2] = (t0 + t1*refVectors[nextSection][2]) + t2*refVectors[section][2];
    enables[0] = enables[1] = enables[2] = 1;
  }

  // S->commutationStep = section;
}

void Brushless::setMotorProps(int polePairs, float leadFactor) {
  this->leadFactor = leadFactor;
  POLE_PAIRS = polePairs;
}

void Brushless::init(uint32_t absPos) {
  encoder.write(encoderCPR - absPos);
  pos_zer = EEPROM.read(0);

  // TODO: Read pole pairs from EEPROM
  countsPerElecRev = encoderCPR / POLE_PAIRS;

  // this is just 25000 for now
  velF.init(0.99, 25000, DLPF_ANGRATE);
}

void Brushless::calibrate(float sweepAmplitude, float convergenceThreshold) {
  // 
  const uint32_t sweepDuration = 200;
  const uint32_t pauseDuration = 500;

  motorEnableFlag = false;
  CommutationType waveSave = waveform;
  float leadSave = leadFactor;
  waveform = SINUSOIDAL;
  leadFactor = 0;
  float vi1, vi2;
  // first stop
  motorEnableFlag = false;
  delay(pauseDuration);
  while(1) {
    delay(pauseDuration);
    amplitude = sweepAmplitude;
    velInt = 0;
    motorEnableFlag = true;
    delay(sweepDuration);
    motorEnableFlag = false;
    // if (velInt < 0) {
    //   // way off, try something quite different
    //   pos_zer = (pos_zer+countsPerElecRev/2)%countsPerElecRev;
    //   continue;
    // }
    vi1 = velInt;

    delay(pauseDuration);
    amplitude = -sweepAmplitude;
    velInt = 0;
    motorEnableFlag = true;
    delay(sweepDuration);
    motorEnableFlag = false;
    vi2 = velInt;

    // float dzero = 0.001*(vi1+vi2);
    float dzero = 0.0002*(vi1+vi2);

    // debug
    Serial1 << "vi1=" << vi1 << ",vi2="<<vi2<< ",dzero="<<dzero << "\n";
    if (fabsf(dzero) < convergenceThreshold) {
      EEPROM.write(0, pos_zer);
      // delay(100);
      break;
    }
    else
      pos_zer = (int)(pos_zer + (int)dzero) % countsPerElecRev;
  }
  waveform = waveSave;
  leadFactor = leadSave;
}

void Brushless::update(float pwmInput) {
  // Figure out amplitude
  switch (mode)
  {
    case VOLTAGE_CONTROL:
      motorEnableFlag = (pwmInput > 0.1 && pwmInput < 0.9);
      amplitude = map(pwmInput, 0.1, 0.9, -1, 1);
      break;
    case POSITION_CONTROL:
      // motorEnableFlag = (pwmInput > 0.1 && pwmInput < 0.9);
      motorEnableFlag = true;
      posDes = -0.35;//map(constrain(pwmInput, 0.1, 0.9), 0.1, 0.9, 0, TWO_PI);
      amplitude = -1 * fmodf_mpi_pi(posRad - posDes);// - 0.01 * motorVel;
      // amplitude = 0.1;
      break;
    case CURRENT_CONTROL:
      // motorEnableFlag = (pwmInput > 0.1 && pwmInput < 0.9);
      // TEST
      amplitude = 0.006 * motorVel;
      break;
    case DEBUGGING:
      // TEST
      motorEnableFlag = true;
      // amplitude = 1;//(millis() < 10000) ? 0.1 : 10000;
      // amplitude = debuggingAmplitude*arm_sin_f32(millis()/2000.0);
      amplitude = debuggingAmplitude;
      // debugging
      // float pwmInput = 0.5 + 0.1 * sin(millis()/1000.0);
      // pwmInput = 0.6;
      break;
  }
}


void Brushless::openLoopTest(uint32_t pause, float amplitude) {
  float highval = 0.5 + 0.5 * amplitude;
  float lowval = 0.5 - 0.5 * amplitude;
  while (true) {
    setOutputs(true, lowval, false, 0.5, true, highval);
    delay(pause);
    setOutputs(false, 0.5, true, lowval, true, highval);
    delay(pause);
    setOutputs(true, highval, true, lowval, false, 0.5);
    delay(pause);
    setOutputs(true, highval, false, 0.5, true, lowval);
    delay(pause);
    setOutputs(false, 0.5, true, highval, true, lowval);
    delay(pause);
    setOutputs(true, lowval, true, highval, false, 0.5);
    delay(pause);
  }
}


float Brushless::posToRadians(uint32_t pos) {
  return ((float)pos * TWO_PI / ((float)encoderCPR));
}

void Brushless::setMotorPhases(float electricalAngle, float amplitude, CommutationType waveform) {
  static uint8_t enables[3];
  static float pwms[3];
  static float amplitudeThresh = 0.01;
  calcSpaceVector(electricalAngle, amplitude, enables, pwms, waveform);
  uint8_t overallEnable = (motorEnableFlag && abs(amplitude)>amplitudeThresh);

  // call the user-defined function
  setOutputs(
    enables[0] && overallEnable, pwms[0], 
    enables[1] && overallEnable, pwms[1], 
    enables[2] && overallEnable, pwms[2]
    );
}


int16_t Brushless::readCurrent() {
  // TODO: Turner
  // if (numCurrentSensors == 2) {
  //   // const uint16_t *vals = analogSyncRead2(is1, is2);
  //   // currents[0] = vals[0];
  //   // currents[1] = vals[1];
  //   currents[0] = analogRead(board.is1);
  //   currents[1] = analogRead(board.is2);

  //   return 0;
  // } else {
  //   // TODO

  //   return 0;
  // } else
  // if (numCurrentSensors == 3) {
  //   // Assuming 3 current sensors
  //   currents[0] = analogRead(board.is1);
  //   currents[1] = analogRead(board.is2);
  //   currents[2] = analogRead(board.is3);

  //   if (waveform == BLOCK) {
  //     // For block/trapezoidal
  //     uint8_t hsphases[] = {2, 2, 1, 1, 0, 0};
  //     uint8_t lsphases[] = {1, 0, 0, 2, 2, 1};

  //     //phase to read current sensor on (high-side FET is on) in {0, 1, 2}
  //     // look at high side, low side
  //     uint8_t hs = hsphases[section];
  //     uint8_t ls = lsphases[section];
  //     if (amplitude < 0)
  //       swapByte(&hs, &ls);

  //     if (currents[ls] > currents[hs])
  //       current = - currents[ls];
  //     else
  //       current = currents[hs];
  //   } else if (waveform == SINUSOIDAL || waveform == TRAPEZOIDAL) {
  //     // current = currents[0] * currents[0] + currents[1] * currents[1] + currents[2] * currents[2];
  //   }

  //   if (amplitude < 0)
  //     current = - current;

  //   return current;
  // }
  return 0;
}
