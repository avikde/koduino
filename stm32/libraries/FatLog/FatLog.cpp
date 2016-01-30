/**
 * @authors Garrett Wenger

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
#include "FatLog.h"

FATFS FatFs;
FIL logFile;
FRESULT code;
UINT bw;
const char *align = "\xaa\xbb";

LoggerStatus FatLog::init(String header, String fmt, uint32_t packetSize) {
  enabled = false;
  this->syncRate = (uint16_t) 500;
  this->packetSize = (UINT) packetSize;

  if (!Clock.isCalibrated()) {
    return LS_RTC_NO_CAL;
  }

  if (SD_Detect() != SD_PRESENT) {
    return LS_NO_DISK;
  }

  if (f_mount(&FatFs, "", 1) == FR_OK) {
    struct tm timeStruct;
    Clock.getTimestruct(&timeStruct);

    // Serial1.begin(115200);
    // Serial1 << "test" << timeStruct.tm_year << "\n";
    uint16_t logNumber = EEPROM.read(LOG_NUMBER_LOCATION);
    EEPROM.write(LOG_NUMBER_LOCATION, (uint16_t) ((logNumber+1)%9999));

    String zeros = "";
    if (logNumber < 10) 
      zeros = "000";
    else if (logNumber < 100)
      zeros = "00";
    else if (logNumber < 1000)
      zeros = "0";
    else
      zeros = "";

    String fileName = "LOG" + zeros + String(logNumber) + ".txt";
    // String fileName = "LOG0000.txt";
    // Serial1 << fileName << "\n";
    if (f_open(&logFile, fileName.c_str(), FA_OPEN_ALWAYS | FA_WRITE) == FR_OK) {
      header += '\n';
      f_write(&logFile, header.c_str(), header.length(), &bw);

      fmt += '\n';
      f_write(&logFile, fmt.c_str(), fmt.length(), &bw);

      f_sync(&logFile);
      lastSync = millis();
      mode = FL_NORMAL;
      return LS_OK;
    }
  }

  return LS_FATFS_ERR;
}

void FatLog::enable(bool flag) {
  if (mode == FL_NORMAL)
    enabled = flag;
}

void FatLog::write(const uint8_t *bytes)
{
  if (mode != FL_NORMAL) return;

  if (enabled) {
    f_write(&logFile, align, 2, &bw);
    f_write(&logFile, bytes, packetSize, &bw);

    if (millis()-lastSync > syncRate) {
      f_sync(&logFile);
      lastSync = millis();
    }
  }
}

void FatLog::setLogNumber(uint16_t logNumber) {
  EEPROM.write(LOG_NUMBER_LOCATION, logNumber);
}
