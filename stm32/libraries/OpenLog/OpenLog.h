/**
 * @authors Avik De <avikde@gmail.com>

  This file is part of koduino <https://github.com/avikde/koduino>

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation, either
  version 3 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, see <http://www.gnu.org/licenses/>.
 */
#ifndef OpenLog_h
#define OpenLog_h

#include <Arduino.h>
#include <stdint.h>

#define MAX_LOG_LINE_LEN 80

typedef enum {
  OL_NOT_INITED, OL_NORMAL, OL_COMMAND
} OpenLogMode;

class OpenLog {
public:
  /**
   * @brief Constructor to set up hardware connections to the OpenLog
   * @details 
   * 
   * @param rst Reset pin
   * @param Ser Connected Serial port
   * @param baud Baud rate (*must match* setting in [CONFIG.TXT](https://github.com/sparkfun/OpenLog/wiki/Config-File) on the SD card)
   */
  OpenLog(uint8_t rst, USARTClass& Ser, uint32_t baud) : mode(OL_NOT_INITED), rst(rst), Ser(Ser), baud(baud), enabled(false) {}

  /**
   * @brief Reset OpenLog and start a new log
   * @details Waits till OpenLog is connected (timeout is 3 seconds). Note that this will 
   * fail if an SD card is not plugged into the socket.
   * 
   * @param header ASCII string containing a comma-separated list of *short* data column names
   * @param fmt ASCII string containing a [python struct format string](https://docs.python.org/2/library/struct.html#format-characters) corresponding to a data row
   * @param packetSize Size of the entire packet
   * @return true if connected, false if failed
   */
  bool init(const char *header, const char *fmt, uint32_t packetSize, bool check);

  bool init(const char *header, const char *fmt, uint32_t packetSize) {
    return init(header, fmt, packetSize, true);
  }

  /**
   * @brief Start / stop logging data
   * @details 
   * 
   * @param flag 
   */
  void enable(bool flag);

  /**
   * @brief Write a data buffer of size `packetSize` specified during init()
   * @details Only writes the data if the Serial TX buffer is empty (i.e. if the previous
   * write operation finished).
   * 
   * @param bytes Pointer to data buffer
   */
  void write(const uint8_t *bytes);

  // //This function pushes OpenLog into command mode
  // void commandMode();
  // //Assumes openlog is in command mode. See openlog wiki for full command set. Sometimes return values (for instance for "size"). portToPushTo can be made 0 if you don't need to print output
  // int query(const char *cmd, int portToPushTo);

protected:
  OpenLogMode mode;
  uint8_t rst;
  USARTClass& Ser;
  uint32_t baud;
  int packetSize;
  bool enabled;
};

#endif
