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
#include <Arduino.h>
#include "PacketParser.h"

void PacketParser::init(int len, uint8_t alignmentLen, const uint8_t *alignment, NewPacketFunc f) {
  this->len = len;
  this->alignmentLen = min(alignmentLen, MAX_ALIGNMENT_LEN);
  for (uint8_t i=0; i<this->alignmentLen; ++i)
    this->alignment[i] = alignment[i];
  this->packetCallback = f;
}

static bool PacketParser::checksum(const uint8_t *buf, int n, ChecksumType type) {
  if (type == CHECKSUM_XOR) {
    uint8_t xorval = 0;
    for (int i=0; i<n; ++i)
      xorval ^= buf[i];
    return (xorval == 0);
  } else if (type == CHECKSUM_16BIT_BIG_ENDIAN) {
    uint16_t checksum = 0;
    for (int tix = 0; tix < n - 2; ++tix)
      checksum += buf[tix];

    uint16_t packetChecksum = (buf[n - 2] << 8) + buf[n - 1];
    return (checksum == packetChecksum);
  }
}
/// BEFORE THIS WAS IN USART

// FIXME: packet parser passes pointer to a "buffer", but problematically this is now a circular buffer

void serialLookForPacket(uint8_t seriali, PacketType *p)
{
  USARTxData * s = serialGetStruct(seriali);
  s->packet = p;

  // Try to keep buffer from ever filling (not circular any more!). rxhead will always be = 0
  s->rxhead = s->rxtail = 0;
}

// Check if the head <len> bytes of the RX buffer starts with alignment
uint8_t bufferStartsWithAlignment(USARTxData *s, volatile PacketType *p)
{
  // Not enough bytes in buffer
  if (s->rxtail < p->len)
    return 0;

  for (int i=0; i<p->alignmentSize; ++i)
  {
    if (s->rxbuf[i] != p->alignment[i])
      return 0;
  }
  // They all matched
  return 1;
}

void packetParserISR(USARTxData *s)
{
  volatile PacketType *p = s->packet;

  // Has a valid packet type been attached?
  if (p != NULL && p->len > 0 && p->newPacketFunc != NULL)
  {
    // Check alignment bytes and if the entire packet is there
    if (bufferStartsWithAlignment(s, p))
    {
      uint8_t accept = p->newPacketFunc((const uint8_t *)&s->rxbuf[0]);
      if (accept)
      {
        // some bytes may have arrived while the ISR was running
        for (int i=0; i < s->rxtail - p->len; ++i)
          s->rxbuf[i] = s->rxbuf[p->len + i];

        s->rxtail -= p->len;
      }
    }
  }
}
