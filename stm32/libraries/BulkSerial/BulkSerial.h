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
#ifndef BulkSerial_h
#define BulkSerial_h

#include <Arduino.h>

struct BulkSerialSettings {
  uint8_t txPin, rxPin;
  USART_TypeDef *USARTx;
  DMA_Channel_TypeDef *chTx, *chRx;
};
extern const BulkSerialSettings MBLC_OPENLOG;
extern const BulkSerialSettings MBLC_RPI;

/**
 * @brief Fixed length bulk serial data transfers using DMA
 * @details No interrupts are used for either RX or TX, but the tradeoff
 * is that the transfer length must be known beforehand. Since the DMA 
 * availability depends on the chip, this is only supported on some USARTSs,
 * and the connections from USART to DMA must be hardcoded here. Settings to use:
 * 
 */
class BulkSerial {
public:
  const BulkSerialSettings& bss;
  uint16_t sizeTx, sizeRx;
  bool enabled;

  BulkSerial(const BulkSerialSettings& bss) : bss(bss), sizeTx(0), sizeRx(0), enabled(false) {}

  void begin(uint32_t baud, uint16_t sizeTx, void *bufTx, uint16_t sizeRx, void *bufRx);
  // user will have to do RX sync

  // to maintain compatibility with OpenLog functionality it can be enabled/disabled. if disabled tx is ignored. in the future it can ignore rx too?
  void enable(bool flag) { enabled = flag; }

  void write();

  // special function to init OpenLog before bulk transmits start
  void initOpenLog(const char *header, const char *fmt);
};

#endif
