
#include "chip.h"
#include "variant.h"
#include "system_clock.h"
#include "exti.h"
#include "pins.h"
#include "gpio.h"
#include "adc.h"

// cxxabi compatibility
extern "C" void __cxa_pure_virtual(void) {
  while(1);
}
void *__dso_handle = NULL;

// Arduino stuff
extern "C"{
void setup() WEAK;
void setup() { }
void loop() WEAK;
void loop() { }
}

// libc_init_array
extern void (*__preinit_array_start []) (void) WEAK;
extern void (*__preinit_array_end []) (void) WEAK;
extern void (*__init_array_start []) (void) WEAK;
extern void (*__init_array_end []) (void) WEAK;
extern void (*__fini_array_start []) (void) WEAK;
extern void (*__fini_array_end []) (void) WEAK;


int main() {
  // This is basically libc_init_array -- handles global constructors
  unsigned int count;
  unsigned int i;
  count = __preinit_array_end - __preinit_array_start;
  for (i = 0; i < count; i++)
    __preinit_array_start[i] ();
  count = __init_array_end - __init_array_start;
  for (i = 0; i < count; i++)
    __init_array_start[i] ();
  count = __fini_array_end - __fini_array_start;
  for (i = 0; i < count; i++)
    __fini_array_start[i] ();


  //4 bits for preemp priority 0 bit for sub priority
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

  // From variant: init peripherals
  variantInit();

  // Timing: micros(), millis(), delay() etc.
  systemClockInit();

  // External interrupts
  interrupts();

  
  setup();
  while (1) {
    loop();
    // if (serialEventRun) serialEventRun();
  }

  return 0;
}

