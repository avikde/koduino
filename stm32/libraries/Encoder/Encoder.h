
#ifndef Encoder_h
#define Encoder_h

#include "timer.h"

/**
 * @brief Quadrature encoder library.
 * @details 
 */
class Encoder {
public:
  /**
   * @brief Initialize the quadrature encoder
   * @details This function configures the pins, and starts the timer connected to them. 
   * **NOTE:** pin1 and pin2 *must* be channels 1 and 2 of hardware timer 2, 3, 4, 5, or 19 on
   * the F373. 
   * To check this, you have to look at the datasheet. Additionally, if not using the 
   * [default pin mapping](@ref Pins), you will have to call pinRemap() *before* calling 
   * this function (see example above).
   * 
   * @param pin1 Pin connected to signal A or B
   * @param pin2 Pin connected to signal B or A
   * @param maxCount The max number of ticks (set according to encoder chip specification). 
   * *Caution:* Counting starts at 0, so for example for a 12-bit encoder set to 4095.
   */
  void init(uint8_t pin1, uint8_t pin2, uint16_t maxCount);

  // // OLD
  // void init(uint8_t timer, uint16_t maxCount, uint8_t pin1, uint8_t af1, uint8_t pin2, uint8_t af2);
  // void init(uint8_t timer, uint16_t maxCount, const TimerPin& p1, const TimerPin& p2);

  /**
   * @brief Read the current encoder count
   * @details 
   * @return 
   */
  uint16_t read(void) {
    return TIMER_MAP[timer].TIMx->CNT;
  }

  /**
   * @brief Set the current encoder count
   * @details Used to (for example) set the "zero" point
   * 
   * @param val 
   */
  void write(uint16_t val) {
    TIMER_MAP[timer].TIMx->CNT = val;
  }

protected:
  uint8_t timer;
};


#endif
