# @authors Avik De <avikde@gmail.com>

# This file is part of koduino <https://github.com/avikde/koduino>

# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation, either
# version 3 of the License, or (at your option) any later version.

# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.

# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, see <http://www.gnu.org/licenses/>.

# Options that are required
ifndef KODUINO_DIR
$(error KODUINO_DIR is not defined!)
endif
ifndef VARIANT
$(error VARIANT is not defined!)
endif

# Options that are optional
UPLOAD_BAUD ?= 230400
UPLOAD_ENTRY ?= dtr_rts
HSE_VALUE ?= 16000000UL
CLKSRC ?= HSI
MAX_PROGRAM_SIZE ?= 0
UPLOAD_METHOD ?= SERIAL
UPLOAD_PORT ?= 
SECTOR_ERASE ?= 0
LIBRARIES ?=
KODUINO_ISRS ?= CALL

######################################################################################

VARIANT_DIR = $(KODUINO_DIR)/variants/$(VARIANT)

# Load variant options
include $(VARIANT_DIR)/variant.mk

# Default options
STD_PERIPH_DIR = $(KODUINO_DIR)/system/$(SERIES)_StdPeriph_Driver
CMSIS_DIR = $(KODUINO_DIR)/system/CMSIS
CORE_DIR = $(KODUINO_DIR)/cores/arduino
LDSCRIPT = $(wildcard $(VARIANT_DIR)/*.ld)
ARCH = arm-none-eabi
CC = $(ARCH)-gcc
CXX = $(ARCH)-g++
LD = $(ARCH)-g++
AS = $(ARCH)-as
AR = $(ARCH)-ar
SZ = $(ARCH)-size

# Try to escape spaces
null := 
SPACE := $(null) $(null)
CURDIR_NOSP  = $(subst $(SPACE),\ ,$(CURDIR))
BUILDDIR  = $(CURDIR_NOSP)/build
# Create obj directory
$(shell   mkdir -p $(BUILDDIR))
PROJNAME  = $(subst $(SPACE),_,$(shell basename $(CURDIR_NOSP)))

# Print some stuff
$(info Compiling for $(VARIANT), MCU = $(MCU), project = $(PROJNAME), upload method is $(UPLOAD_METHOD))

# Compile options
STD_PERIPH_MODULES = adc exti flash gpio i2c misc pwr rcc spi syscfg tim usart dma

# Cygwin needs a prefix, also if not windows, get kernel name
UNAME := 
# ifeq ($(OS),Windows_NT)
# 	PREF := /cygwin64
# else
UNAME := $(shell uname)
PREF := 
# endif

# Libraries
EXTRA_LIB_INCS = $(patsubst %,-I"$(PREF)$(KODUINO_DIR)/libraries/%",$(LIBRARIES))

# Compile and link flags
BFLAGSMCU = -mfpu=fpv4-sp-d16 -mfloat-abi=hard -fsingle-precision-constant -ffast-math -D__FPU_PRESENT=1 -DARM_MATH_CM4 -mcpu=cortex-m4
LDFLAGSMCU = -larm_cortexM4lf_math
# No FPU for M3 or M0
ifeq ($(SERIES), STM32F10x)
	BFLAGSMCU = -mcpu=cortex-m3
	LDFLAGSMCU = 
endif

BFLAGS = -O3 -Os -Wall -Werror-implicit-function-declaration -Wno-sign-compare -Wno-strict-aliasing -Wdouble-promotion -Wno-unused-local-typedefs \
-mlittle-endian -mthumb \
$(BFLAGSMCU) \
-fno-common -fno-exceptions -ffunction-sections -fdata-sections -nostartfiles \
-DHSE_VALUE=$(HSE_VALUE) -DCLKSRC_$(CLKSRC) -DKODUINO_ISRS_$(KODUINO_ISRS) -DMAX_PROGRAM_SIZE=$(MAX_PROGRAM_SIZE) -D$(MCU) -DUSE_STDPERIPH_DRIVER -DSERIES_$(SERIES)

INCFLAGS = -I"$(CMSIS_DIR)/Device/ST/$(SERIES)/Include" -I"$(CMSIS_DIR)/Include" -I"$(STD_PERIPH_DIR)/inc" -I"$(VARIANT_DIR)" -I"$(CORE_DIR)" $(EXTRA_LIB_INCS)

CFLAGS = -std=c99 -Wall $(BFLAGS) $(INCFLAGS)
CXXFLAGS = -std=c++0x -felide-constructors -fno-rtti $(BFLAGS) $(INCFLAGS)

LDFLAGS = $(BFLAGS)  -L$(CMSIS_DIR)/Lib/GCC --static -Wl,--gc-sections -T$(LDSCRIPT) --specs=nano.specs -lnosys -lstdc++ -lm -u _printf_float

STD_PERIPH_SRCS = $(patsubst %,$(STD_PERIPH_DIR)/src/$(STD_PERIPH_PREFIX)_%.c,$(STD_PERIPH_MODULES))
# $(wildcard $(STD_PERIPH_DIR)/src/*.c)

VARIANT_C_SRCS = $(wildcard $(VARIANT_DIR)/*.c)
VARIANT_CXX_SRCS = $(wildcard $(VARIANT_DIR)/*.cpp)
CORE_C_SRCS = $(wildcard $(CORE_DIR)/*.c)
CORE_CXX_SRCS = $(wildcard $(CORE_DIR)/*.cpp)
LIBRARY_C_SRCS = $(foreach dir,$(LIBRARIES),$(wildcard $(KODUINO_DIR)/libraries/$(dir)/*.c))
LIBRARY_CXX_SRCS = $(foreach dir,$(LIBRARIES),$(wildcard $(KODUINO_DIR)/libraries/$(dir)/*.cpp))

C_SRCS = $(VARIANT_C_SRCS) $(CORE_C_SRCS) $(LIBRARY_C_SRCS)
CXX_SRCS = $(VARIANT_CXX_SRCS) $(CORE_CXX_SRCS) $(LIBRARY_CXX_SRCS)
S_SRCS = $(wildcard $(VARIANT_DIR)/*.S)
	
C_OBJS = $(patsubst %.c,$(BUILDDIR)/%.o,$(notdir $(C_SRCS)))
CXX_OBJS = $(patsubst %.cpp,$(BUILDDIR)/%.o,$(notdir $(CXX_SRCS)))
S_OBJS = $(patsubst %.S,$(BUILDDIR)/%.o,$(notdir $(S_SRCS)))

STD_PERIPH_OBJS = $(patsubst %.c,$(BUILDDIR)/%.o,$(notdir $(STD_PERIPH_SRCS)))
OBJS = $(C_OBJS) $(CXX_OBJS) $(S_OBJS)

# Project files
OBJS += $(patsubst %.c,$(BUILDDIR)/%.proj.o,$(notdir $(wildcard $(CURDIR_NOSP)/*.c)))
OBJS += $(patsubst %.cpp,$(BUILDDIR)/%.proj.o,$(notdir $(wildcard $(CURDIR_NOSP)/*.cpp)))

all: flash

# $(KODUINO_DIR)/system/stm32flash/stm32flash -i -rts,-dtr,dtr:rts,-dtr,dtr -w $< -R -b $(UPLOAD_BAUD) -n 1000
flash: $(BUILDDIR)/$(PROJNAME).bin

ifeq ($(UPLOAD_METHOD), SERIAL)
ifneq (,$(findstring MINGW, $(UNAME)))
# stm32ld
	@$(KODUINO_DIR)/system/stm32ld/stm32ld.exe $(UPLOAD_PORT) $(UPLOAD_BAUD) $(UPLOAD_ENTRY) $< 1
else
# stm32loader.py
ifeq ($(strip $(UPLOAD_PORT)),)
ifeq ($(UNAME), Linux)
	@python $(KODUINO_DIR)/system/stm32loader.py -p /dev/ttyUSB -E $(EEP_START) -L $(EEP_LEN) -b $(UPLOAD_BAUD) -y $(UPLOAD_ENTRY) -s $(SECTOR_ERASE) -ew $<
else
	@python $(KODUINO_DIR)/system/stm32loader.py -E $(EEP_START) -L $(EEP_LEN) -b $(UPLOAD_BAUD) -y $(UPLOAD_ENTRY) -s $(SECTOR_ERASE) -ew $<
endif
else
	@python $(KODUINO_DIR)/system/stm32loader.py -p $(UPLOAD_PORT) -E $(EEP_START) -L $(EEP_LEN) -b $(UPLOAD_BAUD) -y $(UPLOAD_ENTRY) -s $(SECTOR_ERASE) -ew $<
endif
endif
else
ifeq ($(UPLOAD_METHOD), NUCLEO)
# nucleo
	@sudo python $(KODUINO_DIR)/system/mbedremounter.py
	@cp $< /Volumes/NUCLEO/
else
# dfu-util
ifeq ($(UNAME),Linux)
	@sudo dfu-util -d 0483:df11 -a 0 -s 0x08000000 -D $<
else
	@dfu-util -d 0483:df11 -a 0 -s 0x08000000 -D $<
endif
endif
endif

$(BUILDDIR)/$(PROJNAME).bin: $(OBJS)
	@echo "[LD] $(@F)"
	@$(LD) $(LDFLAGS) $(OBJS) $(KODUINO_DIR)/system/lib$(SERIES).a -o $(BUILDDIR)/$(PROJNAME).elf $(LDFLAGSMCU)
	@arm-none-eabi-objcopy --output-target=binary $(BUILDDIR)/$(PROJNAME).elf $(BUILDDIR)/$(PROJNAME).bin
	@$(SZ) $(BUILDDIR)/$(PROJNAME).elf | tail -n 1 | awk '{print "Flash: ", $$1+$$2, "\tRAM: ", $$2+$$3}'

disassemble: $(BUILDDIR)/$(PROJNAME).bin
	@$(OBJDMP) -x --syms $(PREF)$< > $(BUILDDIR)/$(PROJNAME).dmp
	@$(OBJDMP) -dS $(PREF)$< > $(BUILDDIR)/$(PROJNAME).dis

# $(info $(UPLOAD_FLAGS))

# Make a static lib to use with the arduino IDE (don't make binary)
# WARNING: the stmxxx_rcc.c file uses the HSE_VALUE for some functions, so those will be
# wrong if the HSE value is changed
lib: $(STD_PERIPH_OBJS)
	@echo "[AR] $(@F)"
	@$(AR) -rcs $(KODUINO_DIR)/system/lib$(SERIES).a $(STD_PERIPH_OBJS)

clean:
	$(shell rm -f $(OBJS) $(BUILDDIR)/main.elf $(BUILDDIR)/main.bin)

# Project
$(BUILDDIR)/%.proj.o: $(CURDIR_NOSP)/%.c
	@echo "[CC] $(@F)"
	@$(CC) $(CFLAGS) -c -o "$(PREF)$@" "$(PREF)$^"
$(BUILDDIR)/%.proj.o: $(CURDIR_NOSP)/%.cpp
	@echo "[CXX] $(@F)"
	@$(CXX) $(CXXFLAGS) -c -o "$(PREF)$@" "$(PREF)$^"

# Libraries
$(BUILDDIR)/%.o: $(KODUINO_DIR)/libraries/*/%.c
	@echo "[CC] $(@F)"
	@$(CC) $(CFLAGS) -c -o "$(PREF)$@" "$(PREF)$^"
$(BUILDDIR)/%.o: $(KODUINO_DIR)/libraries/*/%.cpp
	@echo "[CXX] $(@F)"
	@$(CXX) $(CXXFLAGS) -c -o "$(PREF)$@" "$(PREF)$^"

# Std peripheral libs
$(BUILDDIR)/%.o: $(STD_PERIPH_DIR)/src/%.c
	@echo "[CC] $(^F)"
	@$(CC) $(CFLAGS) -c -o "$@" "$^"

# Variant
$(BUILDDIR)/%.o: $(VARIANT_DIR)/%.c
	@echo "[CC] $(^F)"
	@$(CC) $(CFLAGS) -c -o "$@" "$^"
$(BUILDDIR)/%.o: $(VARIANT_DIR)/%.cpp
	@echo "[CXX] $(^F)"
	@$(CXX) $(CXXFLAGS) -c -o "$@" "$^"
$(BUILDDIR)/%.o: $(VARIANT_DIR)/%.S
	@echo "[AS] $(@F)"
	@$(CC) $(CFLAGS) -c -o "$@" "$^"

# Arduino core
$(BUILDDIR)/%.o: $(CORE_DIR)/%.c
	@echo "[CC] $(^F)"
	@$(CC) $(CFLAGS) -c -o "$@" "$^"
$(BUILDDIR)/%.o: $(CORE_DIR)/%.cpp
	@echo "[CXX] $(^F)"
	@$(CXX) $(CXXFLAGS) -c -o "$@" "$^"


