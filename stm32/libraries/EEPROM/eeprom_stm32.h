/**
 * @authors ST, Avik De <avikde@gmail.com>

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
#ifndef eeprom_stm32_h
#define eeprom_stm32_h

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  These are set to the defaults in ST's EEPROM emulation examples, but should be revised
 *  according to the device. Ideally, it should be placed just under the FLASH top address
 *  to minimize its impact on other code.
 */
#if defined(SERIES_STM32F37x)
  #define PAGE_SIZE  (uint16_t)0x800 // 2K
  #define EEPROM_START_ADDRESS    ((uint32_t)0x08010000) // total = 256K, eeprom after 64K
#elif defined(SERIES_STM32F30x)
  #define PAGE_SIZE  (uint16_t)0x800 // 2K
  #define EEPROM_START_ADDRESS    ((uint32_t)(0x08000000 + MAX_PROGRAM_SIZE)) // specified in boards.txt (end of flash - 2*page size)
#elif defined(STM32F40_41xxx) || defined(STM32F411xE)
  // #define PAGE_SIZE (uint16_t) 0x20000 // 128K
  // #define EEPROM_START_ADDRESS ((uint32_t)0x080C0000)
  // #define PAGE0_ID  FLASH_Sector_10
  // #define PAGE1_ID  FLASH_Sector_11
  #define PAGE_SIZE  (uint16_t)0x4000 // 16K
  #define EEPROM_START_ADDRESS    ((uint32_t)0x08008000) // total = 1M, eeprom after 16K
  #define PAGE0_ID               FLASH_Sector_2
  #define PAGE1_ID               FLASH_Sector_3
  // Device voltage range supposed to be [2.7V to 3.6V], the operation will be done by word
  #define VOLTAGE_RANGE           (uint8_t)VoltageRange_3
#endif


// From ST =======================================================================

/* Pages 0 and 1 base and end addresses */
#define PAGE0_BASE_ADDRESS      ((uint32_t)(EEPROM_START_ADDRESS + 0x000))
#define PAGE0_END_ADDRESS       ((uint32_t)(EEPROM_START_ADDRESS + (PAGE_SIZE - 1)))

#define PAGE1_BASE_ADDRESS      ((uint32_t)(EEPROM_START_ADDRESS + PAGE_SIZE))
#define PAGE1_END_ADDRESS       ((uint32_t)(EEPROM_START_ADDRESS + (2 * PAGE_SIZE - 1)))

/* Used Flash pages for EEPROM emulation */
#define PAGE0                   ((uint16_t)0x0000)
#define PAGE1                   ((uint16_t)0x0001)

/* No valid page define */
#define NO_VALID_PAGE           ((uint16_t)0x00AB)

/* Page status definitions */
#define ERASED                  ((uint16_t)0xFFFF)     /* PAGE is empty */
#define RECEIVE_DATA            ((uint16_t)0xEEEE)     /* PAGE is marked to receive data */
#define VALID_PAGE              ((uint16_t)0x0000)     /* PAGE containing valid data */

/* Valid pages in read and write defines */
#define READ_FROM_VALID_PAGE    ((uint8_t)0x00)
#define WRITE_IN_VALID_PAGE     ((uint8_t)0x01)

/* Page full define */
#define PAGE_FULL               ((uint8_t)0x80)

uint16_t EE_Init(void);
uint16_t EE_ReadVariable(uint16_t VirtAddress, uint16_t* Data);
uint16_t EE_WriteVariable(uint16_t VirtAddress, uint16_t Data);

#ifdef __cplusplus
}
#endif

#endif

