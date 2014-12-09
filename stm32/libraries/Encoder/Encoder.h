
#ifndef Encoder_h
#define Encoder_h

#include "timer.h"

// cldoc:begin-category(Encoder)

// Use a timer as a quadrature decoder. This assumes there is an A/B (x4) 
// quadrature signal, with one of them wired up to TIMx_CH1, the other one
// to TIMx_CH2
class Encoder {
public:

  // 
  void init(uint8_t timer, uint16_t maxCount, uint8_t pin1, uint8_t af1, uint8_t pin2, uint8_t af2);
  // void init(uint8_t timer, uint16_t maxCount, const TimerPin& p1, const TimerPin& p2);

  uint16_t read(void) {
    return TIMER_MAP[timer].TIMx->CNT;
  }

  void write(uint16_t val) {
    TIMER_MAP[timer].TIMx->CNT = val;
  }

protected:
  uint8_t timer;
};


// cldoc:end-category()

#endif
