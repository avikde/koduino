#ifndef OpenLog_h
#define OpenLog_h

#include <Arduino.h>
#include <stdint.h>

/** @addtogroup OpenLog OpenLog driver
 *  @{
 */

#define MAX_LOG_LINE_LEN 80

typedef enum {
  OL_NOT_INITED, OL_NORMAL, OL_COMMAND
} OpenLogMode;

class OpenLog {
public:
  // High level functions for OpenLog v3 firmware attached on some serial port
  // A lot of the functions are from https://github.com/sparkfun/OpenLog/blob/master/firmware/examples/OpenLog_ReadExample/OpenLog_ReadExample.ino

  OpenLog(PinName rst, USARTClass& Ser, uint32_t baud) : mode(OL_NOT_INITED), rst(rst), Ser(Ser), baud(baud), enabled(false) {}

  // Call to reset OpenLog and start a new log
  // Waits till OpenLog is connected (timeout in ms). Returns 1 if connected, 0 if failed
  bool init(const char *header, const char *fmt, uint32_t packetSize);

  // Start / stop logging
  void enable(bool flag);

  // Doesn't actually write the data till TX buffer is clear
  void write(const uint8_t *bytes);

  // //This function pushes OpenLog into command mode
  // void commandMode();
  // //Assumes openlog is in command mode. See openlog wiki for full command set. Sometimes return values (for instance for "size"). portToPushTo can be made 0 if you don't need to print output
  // int query(const char *cmd, int portToPushTo);

protected:
  OpenLogMode mode;
  PinName rst;
  USARTClass& Ser;
  uint32_t baud;
  int packetSize;
  bool enabled;
};

/** @} */ // end of addtogroup

#endif
