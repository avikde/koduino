
#include <Arduino.h>
#include <Encoder.h>

const int led = PC13;
const TimerPin outPin = {PB3, 1, TIMER2, 2};
const TimerPin inPin = {PB5, 2, TIMER3, 2};
const TimerPin encB = {PA0, 2, TIMER5, 1};
const TimerPin encA = {PA1, 2, TIMER5, 2};

Encoder encoder;

void setup() {
  // Serial1.begin(115200);
  serialBegin(SER1, 115200, USART_WordLength_8b, USART_Parity_No, USART_StopBits_1);

  pinMode(led, OUTPUT);

  pinModeTimer(outPin);
  timerInitPWM(TIMER2, 1);

  pinModePulseIn(inPin);

  encoder.init(TIMER5, 1<<12, encB, encA);
}

void loop() {
  digitalWrite(led, TOGGLE);
  analogWrite(outPin, 0.5);

  serialPrintf(SER1, "%d %.2f\n", encoder.read(), pulseIn(inPin));
  delay(100);
}
