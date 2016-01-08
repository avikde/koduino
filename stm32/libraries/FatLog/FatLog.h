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
#ifndef FatLog_h
#define FatLog_h

#include <Arduino.h>
#include <stdint.h>
#include <ff.h>
#include "uSD_SDIO.h"
#include "EEPROM.h"
#include "RTC.h"
#include <time.h>

#define MAX_LOG_LINE_LEN 80
#define LOG_NUMBER_LOCATION 16

// using namespace std;

typedef enum {
  FL_NOT_INITED, FL_NORMAL
} FatLogMode;

typedef enum {
  LS_OK = 0,      /* Log file successfully created */
  LS_RTC_NO_CAL,  /* The RTC is not calibrated */
  LS_NO_DISK,     /* No disk is present */
  LS_FATFS_ERR    /* Error in FatFs */
} LoggerStatus;

class FatLog {
public:
  /**
   * @brief Constructor to set up hardware connections to the OpenLog
   * @details 
   * 
   */
  FatLog() : mode(FL_NOT_INITED), enabled(false) {}

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
  LoggerStatus init(String header, String fmt, uint32_t packetSize);

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

  void setLogNumber(uint16_t logNumber);

protected:
  FatLogMode mode;
  UINT packetSize;
  uint16_t syncRate;
  uint32_t lastSync;
  bool enabled;
};

#endif
