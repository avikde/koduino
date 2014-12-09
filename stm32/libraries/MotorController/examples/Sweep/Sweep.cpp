
#include <Motor.h>

const PinName led = PA8;
const TimerPin pwm1 = {PA0, 11, TIMER19, 1};
// const TimerPin pwm2 = {PA1, 11, TIMER19, 2};
// const TimerPin pwm3 = {PA2, 11, TIMER19, 3};
// const TimerPin pwm4 = {PA3, 11, TIMER19, 4};
const TimerPin pulse1 = {PA6, 2, TIMER3, 1};
// const TimerPin pulse2 = {PB5, 2, TIMER3, 2};
// const TimerPin pulse3 = {PB6, 2, TIMER4, 1};
// const TimerPin pulse4 = {PB7, 2, TIMER4, 2};

const int CONTROL_RATE = 1000;
BlCon34 M1;

void controlLoop() {
  digitalWrite(led, TOGGLE);
  M1.update();

  if (millis() > 1000 && Serial1.writeComplete())
    Serial1 << M1.getPosition() << endl;
}

void setup() {
  Serial1.begin(115200);
  // serialBfloat Kp, float Kd)egin(PA9, PA10, 1, 115200);

  pinMode(led, OUTPUT);

  // PWM output
  timerInitPWM(TIMER19, 1);

  Motor::updateRate = CONTROL_RATE;
  M1.init(pwm1, pulse1, 0, 1);

  M1.setGain(0.3);
  M1.enable(true);

  attachTimerInterrupt(0, controlLoop, CONTROL_RATE);
}

void loop() {
  M1.setPosition(0);
  delay(1000);

  M1.setPosition(1);
  delay(1000);
  // M1.setOpenLoop(0.5);
  // analogWrite(pwm1, 0.55);

}

