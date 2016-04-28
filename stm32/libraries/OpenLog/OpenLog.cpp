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
#include "OpenLog.h"

// #define OLREAD_BUF_SZ 300

const uint8_t OL_ALIGNMENT[] = {0xaa, 0xbb};

bool OpenLog::init(const char *header, const char *fmt, uint32_t packetSize, bool check) {
  enabled = false;
  this->packetSize = packetSize;

  // new: set lower priority
  Ser.irqnPriority = 0xd;

  Ser.begin(baud);

  if (!check) {
    // simple
    delay(500);
    Ser << header << '\n';
    Ser.flush();
    Ser << fmt << '\n';
    mode = OL_NORMAL;
    return true;
  }

  pinMode(rst, OUTPUT);
  digitalWrite(rst, LOW);
  delay(100);
  digitalWrite(rst, HIGH);

  uint32_t start = millis();
  while (millis() - start < 3000) {
    if (Ser.available() && Ser.read() == '<') {
      Ser << header << '\n';
      Ser.flush();
      Ser << fmt << '\n';
      mode = OL_NORMAL;
      return true;
    }
  }
  return false;
}

void OpenLog::enable(bool flag) {
  if (mode == OL_NORMAL)
    enabled = flag;
}

void OpenLog::write(const uint8_t *bytes)
{
  if (mode != OL_NORMAL) return;

  // Transfer as soon as previous one is finished.
  if (enabled && Ser.writeComplete()) {
    Ser.write(OL_ALIGNMENT, 2);
    Ser.write(bytes, packetSize);
  }

  // Time-based
  // uint32_t curTime = millis();
  // if (curTime - olLastLog > olTimeBetweenLogs)
  // {
  //   olLastLog = curTime;

  //   // No alignment bytes...FIXME??
  //   serialWrite(olPort, bytes, olPacketSize);
  //   return 1;
  // }
  // else
  //   return 0;
}




// //This function pushes OpenLog into command mode
// void OpenLog::commandMode() {
//   //Send three control z to enter OpenLog command mode
//   const uint8_t esc = 26;
//   Serial.write(esc);
//   Serial.write(esc);
//   Serial.write(esc);

//   uint32_t start = millis();
//   //Wait for OpenLog to respond with '>' to indicate we are in command mode
//   while (millis() - start < 1000) {
//     if (Ser.available() && Ser.read() == '>') {
//       olcfg.mode = OL_COMMAND;
//       break;
//     }
//   }
// }
// // char tempbuf[1000];

// // FIXME: NOT CONVERTED
// // Run a command mode command and push results to portToPushTo
// int OpenLog::query(const char *cmd, int portToPushTo) {
//   if (olcfg.mode != OL_COMMAND) return 0;

//   static char olReadBuf[OLREAD_BUF_SZ];
//   serialWrite(olcfg.port, (const uint8_t *)cmd, strlen(cmd));

//   //The OpenLog echos the commands we send it by default so we have 'disk\r' sitting 
//   //in the RX buffer. Let's try to not print this.
//   while (1)
//   {
//     if(serialAvailable(olcfg.port))
//       if(serialRead(olcfg.port) == '\r') break;
//   }

//   int bufPos = 0, len = 0;
//   uint8_t endOfRx = 0;
//   while (1)
//   {
//     len = serialAvailable(olcfg.port);
//     if (len > 0)
//     {
//       if (bufPos + len > OLREAD_BUF_SZ) break;

//       // Copy from RX buffer to TX buffer
//       serialReadBytes(olcfg.port, (uint8_t *)&olReadBuf[bufPos], len);
//       bufPos += len;
//       // Check if the last thing was a prompt
//       if (olReadBuf[bufPos-1] == '>')
//       {
//         endOfRx = 1;
//         break;
//       }
//     }

//     delay(1);
//   }
//   // Wait if the OpenLog is still transmitting in case the olReadBuf is not large enough
//   if (!endOfRx)
//   {
//     while (1)
//     {
//       if (serialAvailable(olcfg.port))
//         if (serialRead(olcfg.port) == '>') break;
//     }
//   }

//   if (portToPushTo > 0)
//   {
//     // At 57.6 kbps, expect 57.6/8 ~= 7 bytes/ms => ~150 us / byte
//     const int chunkSize = 5;
//     // The first two bytes seem to be newlines, and the last is the OL prompt, so leave those out
//     for (int i=2; i<bufPos-3; i+=chunkSize)
//     {
//       int toprint = min(chunkSize, bufPos-3-i);
//       serialWrite(portToPushTo, (uint8_t *)&olReadBuf[i], toprint);
//       delay(1);
//     }
//   }

//   // Post processing for return values
//   if (strncmp(cmd, "size", 4) == 0)
//   {
//     return atoi(&olReadBuf[2]);
//   }
//   return 0;
// }
