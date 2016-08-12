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

// Needs to be double buffered for alignment
#define BULKSERIAL_MAX_RX_SIZE 64

/**
 * @brief Presets for using BulkSerial
 * 
 */
struct BulkSerialSettings {
  uint8_t txPin, rxPin;
  USART_TypeDef *USARTx;
  DMA_Channel_TypeDef *chTx, *chRx;
  uint32_t flagRxTc;
};
extern const BulkSerialSettings MBLC_OPENLOG;
extern const BulkSerialSettings MBLC_RPI;

/**
 * @brief Fixed length bulk serial data transfers using DMA
 * @details No interrupts are used for either RX or TX, but the tradeoff
 * is that the transfer length must be known beforehand. Since the DMA 
 * availability depends on the chip, this is only supported on some USARTSs,
 * and the connections from USART to DMA must be hardcoded here. 
 */
class BulkSerial {
public:
  const BulkSerialSettings& bss;
  uint16_t sizeTx, sizeRx;
  bool enabled;

  /**
   * @brief Constructor needs a BulkSerialSettings instance.
   * @details Use predefined `MBLC_OPENLOG` or `MBLC_RPI` for now
   */
  BulkSerial(const BulkSerialSettings& bss) : bss(bss), sizeTx(0), sizeRx(0), enabled(false), curLocalBufIndex(0) {}

  /**
   * @brief Call this in setup 
   * @details [long description]
   * 
   * @param baud Baud rate
   * @param sizeTx Set 0 to disable TX, otherwise specify the packet size in bytes
   * @param bufTx Pointer to TX struct (can pass NULL if sizeTx is 0)
   * @param sizeRx Set 0 to disable RX, otherwise specify the packet size in bytes
   */
  void begin(uint32_t baud, uint16_t sizeTx, void *bufTx, uint16_t sizeRx);

  /**
   * @brief TX can be disabled or enabled (default is disabled)
   * @details This is to maintain compatibility with OpenLog functionality
   * 
   * @param flag enable TX
   */
  void enable(bool flag) { enabled = flag; }

  /**
   * @brief Sends the TX buffer
   */
  void write();

  /**
   * @brief Flag to check if a new packet was received (call this often)
   * @details Once this is called, the flag is reset, and will return 
   * 
   * @param alignment alignment word (remember this is little endian!)
   * @param dest copies the received buffer here starting with alignment on success
   * 
   * @return 1 when there is a new packet, 0 if not new, -1 if it failed to find the alignment 
   */
  int received(uint16_t alignment, uint8_t *dest);

  /**
   * @brief special function to init OpenLog before bulk transmits start
   * 
   * @param header see OpenLog::init()
   * @param fmt see OpenLog::init()
   */
  void initOpenLog(const char *header, const char *fmt);
// protected:
  DMA_InitTypeDef  DMA_InitStructureTx, DMA_InitStructureRx;
  // Need to "double buffer" RX so that when TC happens we can align the packet
  uint8_t localBuf[BULKSERIAL_MAX_RX_SIZE];
  uint8_t curLocalBufIndex;
};

#endif
