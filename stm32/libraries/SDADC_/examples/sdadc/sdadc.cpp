
#include <wirish.h>

SigmaDeltaChannel sdc[] = {SDCH_B14_B15, SDCH_E8_E9, SDCH_B2_B1};
int16_t v[3];

void setup()
{
  uint8_t ret = sigmaDeltaInit(SOFTWARE_START, SIGMADELTA_DIFF, 3, sdc);

  // 
  ledWrite(LED_GREEN, ret);
  ledWrite(LED_RED, !ret);
}

// Use TIM3 or TIM4 Ch1, Ch2, Ch3 to trigger injected conversion on all 3 SDADC

void loop()
{
  sigmaDeltaBeginReadNB(3, sdc);
  // delayMicroseconds(100);

  uint32_t tic = micros();
  sigmaDeltaEndReadNB(3, &sdc[0], v);
  // v[0] = sigmaDeltaRead(sdc[0]);
  // v[1] = sigmaDeltaRead(sdc[1]);
  // v[2] = sigmaDeltaRead(sdc[2]);
  tic = micros() - tic;


  printf("[%lu] %d %d %d\n", tic, v[0], v[1], v[2]);
  delay(50);
}
