/**
  ******************************************************************************
  * @file    STM32F3xx_EEPROM_Emulation/inc/eeprom.h
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    02-October-2012
  * @brief   Main program body.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */ 
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __EEPROM_H
#define __EEPROM_H

// Define for Arduino
#define EEPROM_NUMBER_VALUES 32

/* Includes ------------------------------------------------------------------*/
#include "chip.h"

#define EEPROM_FLASH_SIZE 128

/* Exported constants --------------------------------------------------------*/
/* Define the STM32F3xx Flash page size depending on the used STM32 device */
#define PAGE_SIZE  (uint16_t)0x800  /* Page size = 2KByte */

/* EEPROM start address in Flash */
#define EEPROM_START_ADDRESS    ((uint32_t)0x08000000 + (EEPROM_FLASH_SIZE/2-2)*PAGE_SIZE) // EEPROM emulation start address

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

// Virtual address defined by the user: 0xFFFF value is prohibited
#ifdef EEPROM_NUMBER_VALUES
extern uint16_t eeprom_addresses[EEPROM_NUMBER_VALUES];
#endif

#ifdef __cplusplus
extern "C" {
#endif


/* Exported types ------------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
uint16_t EE_Init(void);
uint16_t EE_ReadVariable(uint16_t VirtAddress, uint32_t* Data);
uint16_t EE_WriteVariable(uint16_t VirtAddress, uint32_t Data);

#ifdef __cplusplus
}
#endif


/**
 * @brief EEPROM library (call with global object `EEPROM`)
 * @details 
 */
class EEPROMClass
{
public:
  EEPROMClass() : inited(false) {}

  /**
   * @brief Read a 32-bit integer from EEPROM
   * @details 
   * 
   * @param address Address between 0 and 31
   * @return 
   */
  uint32_t read(int address);

  /**
   * @brief Read a float from EEPROM
   * @details 
   * 
   * @param address Address between 0 and 31
   * @return 
   */
  float readFloat(int address);

  /**
   * @brief Store a 32-bit integer in EEPROM
   * @details
   * 
   * @param address Address between 0 and 31
   * @param value 
   */
  void write(int address, uint32_t value);

  /**
   * @brief Store a float in EEPROM
   * @details
   * 
   * @param address Address between 0 and 31
   * @param value 
   */
  void writeFloat(int address, float value);
protected:
  bool inited;
  void initCheck();
};

extern EEPROMClass EEPROM;

#endif /* __EEPROM_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
