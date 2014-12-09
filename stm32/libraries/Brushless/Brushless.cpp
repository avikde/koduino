
#include "Brushless.h"
#include <EEPROM.h>


void Brushless::commutate() {
  static uint32_t tic;
  tic = micros();

  // Get position
  pos_act = 4096 - encoder.read();
  motorVel = velF.update(posToRadians(pos_act));
  velInt += motorVel; // Used by calibration routine
  pos_act_01 = pos_act/((float)(1<<12));
  // Get electrical angle
  int pos_mod=(pos_act-pos_zer) % countsPerElecRev;//map to a single electrical revolution
  if (pos_mod < 0)
    pos_mod += countsPerElecRev;
  float elang = pos_mod / ((float)countsPerElecRev);
  
  setMotorPhases(elang, amplitude, SINUSOIDAL);

  // Current sense
  current = readCurrent();

  commutationTime = (1000 + micros() - tic) % 1000;
}

int16_t Brushless::readCurrent() {
  // analogReadBatch(3, isx, currents);
  currents[0] = analogRead(is1);
  currents[1] = analogRead(is2);
  currents[2] = analogRead(is3);

  int16_t currentRaw = 0;

  // For block/trapezoidal
  uint8_t hsphases[] = {2, 2, 1, 1, 0, 0};
  uint8_t lsphases[] = {1, 0, 0, 2, 2, 1};

  //phase to read current sensor on (high-side FET is on) in {0, 1, 2}
  // look at high side, low side
  uint8_t hs = hsphases[section];
  uint8_t ls = lsphases[section];
  if (amplitude < 0)
    swapByte(&hs, &ls);

  if (currents[ls] > currents[hs])
    currentRaw = - currents[ls];
  else
    currentRaw = currents[hs];

  if (amplitude < 0)
    currentRaw = - currentRaw;

  return currentRaw;
}

// void Brushless::dumpCurrentCalibrate(uint32_t duration, float elang) {
//   // disable timer interrupts for "dump current calibration"
//   noTimerInterrupts();
//   delay(100);
//   // setMotorOutputs(0.5, 1, BLOCK);
//   // delay(3000);
//   // setMotorOutputs(0.57, 1, SINUSOIDAL);
//   setMotorPhases(elang, 1, SINUSOIDAL);
//   delay(duration);

//   //Absolute Position (stock)
//   encoder.write(4096 - (uint32_t) (pulseIn(enc0pwm) * (1<<12)));
//   pos_zer = encoder.read() % countsPerElecRev;

//   setMotorPhases(elang, 0, SINUSOIDAL);
  
//   // re-enable timer interrupts for commutation
//   timerInterrupts();
//   return pos_zer;
// }


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
    theta_da = electricalAngle * TWO_PI - ((float)section)*(PI/3);  //Theta, the electrical angle bewteen current angle and section boundary, from 0 to pi/3 
    
    switch (commutation)
    {
      case TRAPEZOIDAL:
      // trapezoidal
        ///81-108us
        //theta_db = sin(theta_da)/sin((PI/3)+theta_da); // Scale factor (in simplest form)
        ///end 81-108us
        ///4us
        theta_db = arm_sin_f32(theta_da)/arm_sin_f32((PI/3)+theta_da); // Scale factor (in simplest form)
        ///end 4us
        ///40ns
        t1 = theta_db*amplitude;            // Time for using vector 1
        t2 = (1-theta_db)*amplitude;        // Time for using vector 2
        t0 = (1-amplitude)/2;               // Time for 0 vector.
        ///end 40ns
        break;
      case SINUSOIDAL:
      // sinusoidal
        // Maximum possible torque assumed
        theta_db = PI/3 - theta_da;                   //pi/3-theta.
        ///55-76us
        //t2 = sin(theta_da)*amplitude;
        //t1 = sin(theta_db)*amplitude;
        ///end 55-76us
        t1 = arm_sin_f32(theta_da)*amplitude;
        t2 = arm_sin_f32(theta_db)*amplitude;
        t0 = (1-amplitude)/2;             // Time for 0 vector.
        break;
      default:
        t1 = 0;
        t2 = 0;
        t0 = 0.5;
    }
    /// 95ns
    nextSection = (section+1)%6;
    pwms[0] = (t0 + t1*refVectors[nextSection][0]) + t2*refVectors[section][0];
    pwms[1] = (t0 + t1*refVectors[nextSection][1]) + t2*refVectors[section][1];
    pwms[2] = (t0 + t1*refVectors[nextSection][2]) + t2*refVectors[section][2];
    /// end 95ns
    enables[0] = enables[1] = enables[2] = 1;
  }

  // S->commutationStep = section;
}

void Brushless::setMotorProps(int polePairs) {
  POLE_PAIRS = polePairs;
}

void Brushless::setCurrentPins(uint8_t is1, uint8_t is2, uint8_t is3) {
  this->is1 = is1;
  this->is2 = is2;
  this->is3 = is3;
}

void Brushless::init(uint32_t freq, float pwmPos) {
  encoder.write(4096 - (uint32_t) (pwmPos * (1<<12)));
  // EE_ReadVariable(0, (uint32_t *)&pos_zer);
  pos_zer = EEPROM.read(0);
  // EE_ReadVariable(1, (uint32_t *)&posctrl0);

  // TODO: Read pole pairs from EEPROM
  countsPerElecRev = 4096 / POLE_PAIRS;

  velF.init(0.99, freq, DLPF_ANGRATE);

}

void Brushless::calibrate() {
  static float sweepAmplitude = 0.1;
  motorEnableFlag = false;
  float vi1, vi2;
  while(1) {
    amplitude = sweepAmplitude;
    velInt = 0;
    motorEnableFlag = true;
    delay(200);
    motorEnableFlag = false;
    if (velInt < 0) {
      // way off, try something quite different
      pos_zer = (pos_zer+countsPerElecRev/2)%countsPerElecRev;
      continue;
    }
    vi1 = velInt;
    delay(100);

    amplitude = -sweepAmplitude;
    velInt = 0;
    motorEnableFlag = true;
    delay(200);
    motorEnableFlag = false;
    vi2 = velInt;
    delay(100);

    float dzero = 0.001*(vi1+vi2);
    if (abs(dzero) < 1) {
      EEPROM.write(0, pos_zer);
      // delay(100);
      break;
    }
    else
      pos_zer = (int)(pos_zer + (int)dzero) % countsPerElecRev;
  }
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
      motorEnableFlag = (pwmInput > 0.1 && pwmInput < 0.9);
      posDes = 0;//map(constrain(pwmInput, 0.1, 0.9), 0.1, 0.9, 0, TWO_PI);
      amplitude = -1 * fmodf_mpi_pi(posToRadians(pos_act) - posctrl0 - posDes);// - 0.01 * motorVel;

      // // Setting zero
      // if (digitalRead(SW1) == LOW)
      // {
      //   posctrl0 = posToRadians(pos_act);
      //   digitalWrite(LED_YELLOW, LOW);
      //   EE_WriteVariable(1, *(uint32_t *)&posctrl0);
      // }
      // else
      //   digitalWrite(LED_YELLOW, HIGH);
      break;
    case CURRENT_CONTROL:
      motorEnableFlag = (pwmInput > 0.1 && pwmInput < 0.9);
      // TEST
      // amplitude = 0.4;
      break;
    case DEBUGGING:
      // TEST
      motorEnableFlag = true;
      // amplitude = 1;//(millis() < 10000) ? 0.1 : 10000;
      // amplitude = arm_sin_f32(millis()/1000.0);
      amplitude = 0.3;
      // debugging
      // float pwmInput = 0.5 + 0.1 * sin(millis()/1000.0);
      // pwmInput = 0.6;
      break;
  }
}


void Brushless::openLoopTest(uint32_t pause) {
  while (true) {
    setMotorPhases(HIGH, 0, LOW, 0.5, HIGH, 1);
    delay(pause);
    setMotorPhases(LOW, 0.5, HIGH, 0, HIGH, 1);
    delay(pause);
    setMotorPhases(HIGH, 1, HIGH, 0, LOW, 0.5);
    delay(pause);
    setMotorPhases(HIGH, 1, LOW, 0.5, HIGH, 0);
    delay(pause);
    setMotorPhases(LOW, 0.5, HIGH, 1, HIGH, 0);
    delay(pause);
    setMotorPhases(HIGH, 0, HIGH, 1, LOW, 0.5);
    delay(pause);
  }
}


float Brushless::posToRadians(uint32_t pos) {
  return ((float)pos * 0.00153398078); // 2*pi/4096
}


void BrushlessPwmEn::setMotorPhases(float electricalAngle, float amplitude, CommutationType waveform) {
  static uint8_t enables[3];
  static float pwms[3];
  static float amplitudeThresh = 0.01;
  calcSpaceVector(electricalAngle, amplitude, enables, pwms, waveform);
  uint8_t overallEnable = (motorEnableFlag && abs(amplitude)>amplitudeThresh);
  digitalWrite(en1, (enables[0] && overallEnable) ? HIGH : LOW);
  digitalWrite(en2, (enables[1] && overallEnable) ? HIGH : LOW);
  digitalWrite(en3, (enables[2] && overallEnable) ? HIGH : LOW);
  analogWrite(pwm1, pwms[0]);
  analogWrite(pwm2, pwms[1]);
  analogWrite(pwm3, pwms[2]);
}

void BrushlessPwmEn::setMotorPhases(LogicValue e1, float p1, LogicValue e2, float p2, LogicValue e3, float p3) {
  digitalWrite(en1, e1);
  digitalWrite(en2, e2);
  digitalWrite(en3, e3);
  analogWrite(pwm1, p1);
  analogWrite(pwm2, p2);
  analogWrite(pwm3, p3);
}

