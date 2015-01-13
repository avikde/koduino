
**This guide has not been completed yet, please check back soon**

---

The steps below are cumulative. For example, if you need to do a type 3 port, you would also need to do steps 1-2. Consequently, it is the easiest to do port to a different package for an existing chip.

### Identifying your chip

The STM32 chip part numbers are of the form:

    STM32|<CHIP_ID>|<PACKAGE>|<FLASH_SIZE>

For example, for `STM32F405RG`: `CHIP_ID = F405`, `PACKAGE = R`, `FLASH_SIZE = G`


| Series | MCU | Chip IDs |
|---|---|---|
| STM32F30x | STM32F37x |  |
| STM32F37x | STM32F37x | F373, F378 |
| STM32F4xx | STM32F401xxx | F405, F407, F417 |
| STM32F4xx | STM32F40_41xxx | F405, F407, F417 |
| STM32F411 | STM32F411xE | F411 |
| STM32F411 | F405, ... |
| Set options (HSE value, board variant, libraries) through IDE | Set options in Makefile |
| Recompiles every time (bug?) | Faster to recompile |
| Uses stm32flash to upload | Uses stm32loader.py to upload |
|  | Not recommended on Windows |

Choose:

## 1. New package

## 2. New variant

A variant corresponds to a subdirectory of `koduino/variants`. You need a new variant in the following cases:

* If a different startup script is required, a new variant should be defined. E.g. within the F4 series, F40_41x and F411x need different variants. This is actually a necessity, since if you put different startup .S scripts in the variant directory, the Arduino IDE will compile all files in the directory.
* Usually a different memory size 

## 2. New memory size for existing chip

Need new startup .S file

## 3. New chip in existing series



## 4. New series




