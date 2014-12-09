
#include <stdlib.h>
#include <string.h>
#include "ocu.h"
#include "exti.h"
#include "timebase.h"
#include "usart.h"
#include "system_clock.h"
#include "devices/openlog.h"


// how much serial data we expect before a newline (console)
#define CONSOLE_MAX_INPUT 50

// Public configuration (extern)
OCUConfig ocu = {
  .port = 1,
  .button = NULL,
  .dPad = NULL,
  .enable = NULL
};

// PUBLIC FUNCTIONS ======================================================

void ocuInit(PinName tx, PinName rx, int port, uint32_t baud, KeyPressFunc button, KeyPressFunc dPad, EnableDisableFunc enable)
{
  ocu.port = port;
  ocu.button = button;
  ocu.dPad = dPad;
  ocu.enable = enable;
  // XBee bootloading, 8E1
  serialBeginAs(tx, rx, port, baud, SERIAL_8E1);
}

// Listen for commands. Returns quickly (doesn't wait), unless it gets put in console mode
void ocuCheckRX()
{
  while (serialAvailable(ocu.port) > 0)
  {
    // XBee command is 1 byte, so just read 1 byte at a time
    uint8_t rxbyte = serialRead(ocu.port);
    switch (rxbyte)
    {
      // the 4 main buttons + 2 bumpers + 2 triggers
      case 0 ... 7:
        if (ocu.button != NULL) ocu.button(ocu.port, rxbyte);
        break;
      // 8 and 9 are the little buttons in the middle
      case 9:
        ocuConsole();
        break;
      case 12 ... 15:
        if (ocu.dPad != NULL) ocu.dPad(ocu.port, rxbyte);
        break;
      default:
        break;
    }
  }
}

void ocuConsole()
{
  serialPrintf(ocu.port, "Entering console mode...\n");

  // disable motors, external and timer interrupts
  if (ocu.enable != NULL) ocu.enable(0);
  noTimerInterrupts();
  noInterrupts();
  if (olcfg.mode != OL_NOT_PRESENT)
  {
    openlogReset();
    delay(100);
    openlogCommandMode();
    delay(100);
  }

  ocuConsolePrompt();

  // Loop
  while (1)
  {
    if (serialAvailable(ocu.port) > 0)
    {
      if (ocuConsoleNewByte(serialRead(ocu.port)) == CONSOLE_EXIT)
        break;
    }
  }

  // Go back to regular mode
  timerInterrupts();
  interrupts();
  if (olcfg.mode != OL_NOT_PRESENT)
  {
    openlogReset();
  }
  if (ocu.enable != NULL) ocu.enable(1);
}

// PRIVATE FUNCTIONS TO IMPLEMENT THE CONSOLE ================================= 

void ocuConsolePrompt()
{
  serialPrintf(ocu.port, "> ");
}

OCUConsoleReturnType ocuConsoleCommand(const char *cmd)
{
  // // Test echo
  // serialPrintf(ocu.port, cmd);

  // Check commands
  if (strcmp(cmd, "millis") == 0)
    serialPrintf(ocu.port, "%lu\n", millis());
  else if (strcmp(cmd, "micros") == 0)
    serialPrintf(ocu.port, "%lu\n", micros());
  else if (strcmp(cmd, "disk") == 0)
    openlogQuery("disk\r", ocu.port);
  else if (strlen(cmd) > 5 && (strncmp(cmd, "size", 4) == 0 || strncmp(cmd, "read", 4) == 0))
  {
    int num = atoi(&cmd[4]);
    char olcmd[50];
    sprintf(olcmd, "size LOG%05d.TXT\r", num);
    int sz = openlogQuery(olcmd, 0);
    if (strncmp(cmd, "size", 4) == 0)
      serialPrintf(ocu.port, "%d\n", sz);
    else
    {
      //
      serialPrintf(ocu.port, "==");
      delay(500);

      // Read in chunks
      int start = 0;
      const int olReadChunkSz = 500;
      while (start < sz)
      {
        int toread = min(olReadChunkSz, sz - start);
        sprintf(olcmd, "read LOG%05d.TXT %d %d 3\r", num, start, toread);
        openlogQuery(olcmd, ocu.port);
        start += olReadChunkSz;
      }
    }
  }
  else if (strcmp(cmd, "ls") == 0)
    openlogQuery("ls\r", ocu.port);
  else if (strcmp(cmd, "exit") == 0)
    return CONSOLE_EXIT;
  else
  {
    serialPrintf(ocu.port, "Invalid command. Try one of:\n");
    serialPrintf(ocu.port, "exit\t\tClose console\n");
    serialPrintf(ocu.port, "olls\t\tOpenLog ls\n");
    serialPrintf(ocu.port, "olreset\t\tReset OpenLog\n");
    serialPrintf(ocu.port, "olcmd\t\tPut OpenLog in command mode\n");
    serialPrintf(ocu.port, "cp <num>\tCopy log <num> (int) from robot\n");
  }

  ocuConsolePrompt();
  return CONSOLE_CONTINUE;
}

OCUConsoleReturnType ocuConsoleNewByte(const uint8_t inByte)
{
  static char inputLine[CONSOLE_MAX_INPUT];
  static unsigned int inputPos = 0;

  switch (inByte)
  {
  case '\n':   // end of text
    inputLine[inputPos] = 0;  // terminating null byte
    
    // terminator reached! process input_line here ...
    if (ocuConsoleCommand(inputLine) == 1)
      return CONSOLE_EXIT;
    
    // reset buffer for next time
    inputPos = 0;  
    break;

  case '\r':   // discard carriage return
    break;

  default:
    // keep adding if not full ... allow for terminating null byte
    if (inputPos < (CONSOLE_MAX_INPUT - 1))
      inputLine [inputPos++] = inByte;
    break;
  }  // end of switch
 
  return CONSOLE_CONTINUE;
} // end of processIncomingByte 

