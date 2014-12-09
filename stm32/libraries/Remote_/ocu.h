#ifndef _ocu_h_
#define _ocu_h_

#include <stdint.h>
#include "board.h"

// Drivers to help the XBee act as a:
// 1) remote control (joystick) receiver
// 2) create a simple "console"
// 3) parse console commands such as
// "openlog <openlog query>" (passed straight to openlog)
// others?

typedef void (*KeyPressFunc)(int port, uint8_t button);
typedef void (*EnableDisableFunc)(uint8_t enable);

typedef struct {
  int port;
  // Callback for keypresses on the joystick
  KeyPressFunc button, dPad;
  // Callback for disabling motors and such to go to console mode
  EnableDisableFunc enable;
} OCUConfig;
extern OCUConfig ocu;

void ocuInit(PinName tx, PinName rx, int port, uint32_t baud, KeyPressFunc button, KeyPressFunc dPad, EnableDisableFunc enable);
void ocuCheckRX();
// Usually triggered by a received command, but could be triggered otherwise.
void ocuConsole();

// PRIVATE ==========================================================

// Private console loop function return type
typedef enum {
  CONSOLE_CONTINUE, CONSOLE_EXIT
} OCUConsoleReturnType;

void ocuConsolePrompt();
OCUConsoleReturnType ocuConsoleCommand(const char *cmd);
OCUConsoleReturnType ocuConsoleNewByte(const uint8_t inByte);

#endif
