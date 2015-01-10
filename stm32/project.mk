
# Options that are required
ifndef KODUINO_DIR
$(error KODUINO_DIR is not defined!)
endif
ifndef HSE_VALUE
$(error HSE_VALUE is not defined!)
endif
ifndef VARIANT
$(error VARIANT is not defined!)
endif

# Options that are optional
ifndef UPLOAD_BAUD
UPLOAD_BAUD = 57600
endif
ifndef HSE_CLOCK
HSE_CLOCK = 0
endif

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
BUILDDIR  = $(CURDIR)/build
# Create obj directory
$(shell   mkdir -p $(BUILDDIR))
PROJNAME  = $(shell basename $(CURDIR))

# Compile options
STD_PERIPH_MODULES = adc exti flash gpio i2c misc pwr rcc spi syscfg tim usart

# Cygwin needs a prefix
ifeq ($(OS),Windows_NT)
	PREF := /cygwin64
else
	PREF := 
endif

# Libraries
EXTRA_LIB_INCS = $(patsubst %,-I$(PREF)$(KODUINO_DIR)/libraries/%,$(LIBRARIES))

# Compile and link flags
BFLAGS = -O3 -Os -Wall -Werror-implicit-function-declaration -Wno-sign-compare -Wno-strict-aliasing -Wdouble-promotion -Wno-unused-local-typedefs \
-mcpu=cortex-m4 -mlittle-endian -mthumb \
-mfpu=fpv4-sp-d16 -mfloat-abi=hard -fsingle-precision-constant -ffast-math -D__FPU_PRESENT=1 -DARM_MATH_CM4 \
-fno-common -fno-exceptions -ffunction-sections -fdata-sections -nostartfiles \
-DHSE_VALUE=$(HSE_VALUE) -D$(MCU) -DUSE_STDPERIPH_DRIVER -DSERIES=$(SERIES)

INCFLAGS = -I$(CMSIS_DIR)/Device/ST/$(SERIES)/Include -I$(CMSIS_DIR)/Include -I$(STD_PERIPH_DIR)/inc -I$(VARIANT_DIR) -I$(CORE_DIR) $(EXTRA_LIB_INCS)

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
OBJS += $(patsubst %.c,$(BUILDDIR)/%.proj.o,$(notdir $(wildcard $(CURDIR)/*.c)))
OBJS += $(patsubst %.cpp,$(BUILDDIR)/%.proj.o,$(notdir $(wildcard $(CURDIR)/*.cpp)))

# $(info $(INCFLAGS))

all: flash

# $(KODUINO_DIR)/system/stm32flash/stm32flash -i -rts,-dtr,dtr:rts,-dtr,dtr -w $< -R -b $(UPLOAD_BAUD) -n 1000
flash: $(BUILDDIR)/$(PROJNAME).bin
ifeq ($(UPLOAD_METHOD), SERIAL)
	@python $(KODUINO_DIR)/system/stm32loader.py -b $(UPLOAD_BAUD) -E $(EEP_START) -L $(EEP_LEN) -ew $<
else
ifeq ($(UNAME),Linux)
	@sudo dfu-util -d 0483:df11 -a 0 -s 0x08000000 -D $<
else
	@dfu-util -d 0483:df11 -a 0 -s 0x08000000 -D $<
endif
endif

$(BUILDDIR)/$(PROJNAME).bin: $(OBJS)
	@echo "[LD] $(@F)"
	@$(LD) $(LDFLAGS) $(OBJS) $(KODUINO_DIR)/system/lib$(SERIES).a -o $(BUILDDIR)/$(PROJNAME).elf -larm_cortexM4lf_math
	@arm-none-eabi-objcopy --output-target=binary $(BUILDDIR)/$(PROJNAME).elf $(BUILDDIR)/$(PROJNAME).bin
	@$(SZ) $(BUILDDIR)/$(PROJNAME).elf | tail -n 1 | awk '{print "Flash:\t", $$1+$$2, "\t[",($$1+$$2)*100/(256*1024),"% ]", "\nRAM:\t", $$2+$$3, "\t[",($$2+$$3)*100/(32*1024),"% ]"}'

disassemble: $(BUILDDIR)/$(PROJNAME).bin
	@$(OBJDMP) -x --syms $(PREF)$< > $(BUILDDIR)/$(PROJNAME).dmp
	@$(OBJDMP) -dS $(PREF)$< > $(BUILDDIR)/$(PROJNAME).dis

# $(info $(STD_PERIPH_OBJS))

# Make a static lib to use with the arduino IDE (don't make binary)
# WARNING: the stmxxx_rcc.c file uses the HSE_VALUE for some functions, so those will be
# wrong if the HSE value is changed
lib: $(STD_PERIPH_OBJS)
	@echo "[AR] $(@F)"
	@$(AR) -rcs $(KODUINO_DIR)/system/lib$(SERIES).a $(STD_PERIPH_OBJS)

clean:
	$(shell rm -f $(OBJS) $(BUILDDIR)/main.elf $(BUILDDIR)/main.bin)

# Project
$(BUILDDIR)/%.proj.o: $(CURDIR)/%.c
	@echo "[CC] $(@F)"
	@$(CC) $(CFLAGS) -c -o $(PREF)$@ $(PREF)$^
$(BUILDDIR)/%.proj.o: $(CURDIR)/%.cpp
	@echo "[CXX] $(@F)"
	@$(CXX) $(CXXFLAGS) -c -o $(PREF)$@ $(PREF)$^

# Libraries
$(BUILDDIR)/%.o: $(KODUINO_DIR)/libraries/*/%.c
	@echo "[CC] $(@F)"
	@$(CC) $(CFLAGS) -c -o $(PREF)$@ $(PREF)$^
$(BUILDDIR)/%.o: $(KODUINO_DIR)/libraries/*/%.cpp
	@echo "[CXX] $(@F)"
	@$(CXX) $(CXXFLAGS) -c -o $(PREF)$@ $(PREF)$^

# Std peripheral libs
$(BUILDDIR)/%.o: $(STD_PERIPH_DIR)/src/%.c
	@echo "[CC] $(^F)"
	@$(CC) $(CFLAGS) -c -o $@ $^

# Variant
$(BUILDDIR)/%.o: $(VARIANT_DIR)/%.c
	@echo "[CC] $(^F)"
	@$(CC) $(CFLAGS) -c -o $@ $^
$(BUILDDIR)/%.o: $(VARIANT_DIR)/%.cpp
	@echo "[CXX] $(^F)"
	@$(CXX) $(CXXFLAGS) -c -o $@ $^
$(BUILDDIR)/%.o: $(VARIANT_DIR)/%.S
	@echo "[AS] $(@F)"
	@$(CC) $(CFLAGS) -c -o $@ $^

# Arduino core
$(BUILDDIR)/%.o: $(CORE_DIR)/%.c
	@echo "[CC] $(^F)"
	@$(CC) $(CFLAGS) -c -o $@ $^
$(BUILDDIR)/%.o: $(CORE_DIR)/%.cpp
	@echo "[CXX] $(^F)"
	@$(CXX) $(CXXFLAGS) -c -o $@ $^


