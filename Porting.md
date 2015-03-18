
**This guide is a work in progress**

---

The steps below are cumulative. For example, if you need to do a type 3 port, you would also need to do steps 1-2. Consequently, it is the easiest to do port to a different package for an existing chip.

### Identifying your chip

The STM32 chip part numbers are of the form:

    STM32|<CHIP_ID>|<PACKAGE>|<FLASH_SIZE>

For example, for `STM32F405RG`: `CHIP_ID = F405`, `PACKAGE = R`, `FLASH_SIZE = G`

In the language of the Koduino libraries (and the Arduino 1.5+ hardware spec) there are only "variants". Some variants and (a non-exclusive list of) chips that fall under it are listed below:

| Variant (in Koduino libraries) | Series | MCU | Chip IDs |
|---|---|---|---|
| f10x (not working yet) | - | - |  |
| f30x (not working yet) | STM32F30x | STM32F30x | F303, ... |
| f37x | STM32F37x | STM32F37x | F373, F378 |
| f40_41x | STM32F4xx | STM32F40_41xxx | F405, F407, F417 |
| f411 (not working yet) | STM32F4xx | STM32F411xE | F411 |
| f429_439 (not working yet) | STM32F4xx | STM32F429_439xx | F429, F439 |

Choose:

## 1. New package


## 2. New variant

A variant corresponds to a subdirectory of `koduino/variants`. You need a new variant in the following cases:

* If a different startup script is required, a new variant should be defined. E.g. within the F4 series, F40_41x and F411x need different variants. This is actually a necessity, since if you put different startup .S scripts in the variant directory, the Arduino IDE will compile all files in the directory.
* Usually a different memory size 

### 2.1 Generating variant.cpp

Unfortunately, which pins are able use `analogRead`, which timers need to be activated for `analogWrite` needs to be manually set up for a new chip. 

I find that this process is made easier with spreadsheets, examples of which are in the table below:

| Variant | Pin map | Timer map |
|---|---|---|
| f37x (done manually) | - | - |
| f40_41x | [f40_41x_PIN_MAP](https://docs.google.com/spreadsheet/pub?key=0Ai-vm-to9OcDdG1zMzR5WFhweGVwNlNnZmtQdlpsb2c&single=true&gid=5&output=html) | [f40_41x_TIMER_MAP](https://docs.google.com/spreadsheet/pub?key=0Ai-vm-to9OcDdG1zMzR5WFhweGVwNlNnZmtQdlpsb2c&single=true&gid=8&output=html) |
| f411 (needs to be checked) | [f411_PIN_MAP](https://docs.google.com/spreadsheet/pub?key=0Ai-vm-to9OcDdG1zMzR5WFhweGVwNlNnZmtQdlpsb2c&single=true&gid=9&output=html) | [f411_TIMER_MAP](https://docs.google.com/spreadsheet/pub?key=0Ai-vm-to9OcDdG1zMzR5WFhweGVwNlNnZmtQdlpsb2c&single=true&gid=10&output=html) |
| f10x (in progress) | [f10x_PIN_MAP](https://docs.google.com/spreadsheet/pub?key=0Ai-vm-to9OcDdG1zMzR5WFhweGVwNlNnZmtQdlpsb2c&single=true&gid=11&output=html) | [f10x_TIMER_MAP](https://docs.google.com/spreadsheet/pub?key=0Ai-vm-to9OcDdG1zMzR5WFhweGVwNlNnZmtQdlpsb2c&single=true&gid=12&output=html) |

Follow these steps:

* Create a spreadsheet pair as above (f40_41x is the best example) for the targeted chip using the datasheet
* To go with these spreadsheets, there is a python script called `variant_codegen.py` in the `variants` directory which prints out C code that can be pasted into `variant.cpp` and `variant.h`
* The USARTx_TX, RX pins need to be manually entered in the definition of `USART_MAP`
* `TIMEBASE_MAP` configures timers used for [attachTimerInterrupt](@ref Timebase). At least one of them *must* be configured for the system clock (`millis()`, `micros()`, `delay()` will not work otherwise).
* Set `SYSCLK_TIMEBASE` to the array index into `TIMEBASE_MAP` that should be the system clock. The actual timer peripheral that should be used (can be a basic timer, per ST datasheet) is configured in the first section of `variantInit()`.
* The interrupt handlers *must* be configured in `stm32fxxx_it.c`. At a minimum, make sure that the system clock ISR is pointing to the correct timer. For an example, please see an existing variant such as `f37x` or `f40_41x`.

## 3. New memory size for existing chip

Need new startup .S file

## 4. New chip in existing series


## 5. New series




