
#include <MinitaurLeg.h>

const PinName led = PC13;
const TimerPin pwm1 = {PA0, 2, TIMER5, 1};
const TimerPin pwm2 = {PA11, 2, TIMER5, 2};
// const TimerPin pwm3 = {PA2, 2, TIMER5, 3};
const TimerPin pwm4 = {PA3, 2, TIMER5, 4};
const TimerPin pwm7 = {PA14, 10, TIMER12, 1};
const PinName pulse1 = PA6;
const PinName pulse2 = PB5;
// const TimerPin pulse3 = {PB6, 2, TIMER4, 1};
const PinName pulse4 = PB7;
const PinName pulse7 = PA15;

const int CONTROL_RATE = 1000;
BlCon34 M1;
BlCon34 M2;
MinitaurLeg leg1(&M1,&M2);
uint32_t tic = 0;

#define PHI 0
#define LENGTH 1

void debug() {
  //Serial1 << M1.getPosition() << "\t" << M2.getPosition() << "\n";
  Serial1 << leg1.getPosition(PHI) << '\t' << leg1.getPosition(LENGTH) << '\n';
}

void controlLoop() {
  digitalWrite(led, TOGGLE);
  leg1.update();

  leg1.setPosition(PHI, 0);
  leg1.setPosition(LENGTH, 0.5);
  // M1.update();
  //M2.update();
}

void setup() {
  Serial1.begin(115200);

  pinMode(led, OUTPUT);

  // PWM output
  timerInitPWM(TIMER5, 1);
  timerInitPWM(TIMER12, 1);

  Motor::updateRate = CONTROL_RATE;
  M1.init(pwm1, pulse1, -0.76, -1);
  M2.init(pwm2, pulse2, 2.81, 1);//0 is zero position, 1 is direction

  // legs
  leg1.setGain(PHI, 1.5);
  leg1.setGain(LENGTH, 0.1);
  leg1.enable(true);

  attachTimerInterrupt(0, controlLoop, CONTROL_RATE);
  attachTimerInterrupt(1, debug, 100);
  delay(1000);
}

void loop() {
  //M2.enable(true);
  //M2.setOpenLoop(-0.1);

  // delay(1000);

  // M1.setPosition(1);
  // delay(1000);
  // M1.setOpenLoop(0.1);
  // analogWrite(pwm1, 0.55);
}

