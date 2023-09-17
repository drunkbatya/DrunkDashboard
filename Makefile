# Tools
PREFIX = arm-none-eabi-
CC = $(PREFIX)gcc
AS = $(PREFIX)gcc -x assembler-with-cpp
CP = $(PREFIX)objcopy
SZ = $(PREFIX)size
HEX = $(CP) -O ihex
BIN = $(CP) -O binary -S
PY = python3
ASSETS_COMPILLER = scripts/icon_convert.py

# Vars
TARGET = DrunkDashboard
BUILD_DIR = build
ASSETS_DIR = assets
ASSETS_SOURCE_DIR = $(ASSETS_DIR)/src
ASSETS_BUILD_DIR = $(ASSETS_DIR)/build
SRCDIR = src
CPU = -mcpu=cortex-m7
FPU = -mfpu=fpv5-d16
FLOAT-ABI = -mfloat-abi=hard
MCU = $(CPU) -mthumb $(FPU) $(FLOAT-ABI)
DEBUG = 1
OPT = -Og
LDSCRIPT = stm/STM32H743VITx_FLASH.ld
MAKEFLAGS+="-j -l $(shell sysctl hw.ncpu | awk '{print $2}') "
WARN = -Wall -Werror -Wextra

AS_DEFS =

C_DEFS = \
	-DUSE_FULL_LL_DRIVER \
	-DHSE_VALUE=25000000 \
	-DHSE_STARTUP_TIMEOUT=100 \
	-DLSE_STARTUP_TIMEOUT=5000 \
	-DLSE_VALUE=32768 \
	-DEXTERNAL_CLOCK_VALUE=12288000 \
	-DHSI_VALUE=64000000 \
	-DLSI_VALUE=32000 \
	-DVDD_VALUE=3300 \
	-DSTM32H743xx

AS_INCLUDES =

C_INCLUDES = \
	-Isrc \
	-Ilib \
	-Iassets/build \
	-Istm/Core/Inc \
	-Istm/Drivers/STM32H7xx_HAL_Driver/Inc \
	-Istm/Drivers/CMSIS/Device/ST/STM32H7xx/Include \
	-Istm/Drivers/CMSIS/Include

ASFLAGS = $(MCU) $(AS_DEFS) $(AS_INCLUDES) $(OPT) $(WARN) -fdata-sections -ffunction-sections

CFLAGS += $(MCU) $(C_DEFS) $(C_INCLUDES) $(OPT) $(WARN) -fdata-sections -ffunction-sections

ifeq ($(DEBUG), 1)
CFLAGS += -g -gdwarf-2
endif

CFLAGS += -MMD -MP -MF"$(@:%.o=%.d)"

LIBS = -lc -lm -lnosys
LIBDIR =
LDFLAGS = $(MCU) -specs=nano.specs -T$(LDSCRIPT) $(LIBDIR) $(LIBS) -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref -Wl,--gc-sections

ASSETS_SOURCES = $(shell find $(ASSETS_SOURCE_DIR) -type f -name '*.png')

HEADERS = $(shell find $(SRCDIR) -type f -name '*.h')

ifneq ($(ASSETS_SOURCES),)
C_SOURCES = $(ASSETS_BUILD_DIR)/assets_icons.c
endif

C_SOURCES += \
	stm/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_ll_gpio.c \
	stm/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_ll_utils.c \
	stm/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_ll_usart.c \
	stm/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_ll_rcc.c \
	stm/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_ll_tim.c \
	stm/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_ll_spi.c
C_SOURCES += $(wildcard src/*.c)
C_SOURCES += $(wildcard src/scenes/*.c)
C_SOURCES += $(wildcard src/views/*.c)
C_SOURCES += $(wildcard src/halk/*.c)
C_SOURCES += $(wildcard stm/Core/Src/*.c)
C_SOURCES += $(wildcard lib/u8g2/*.c)

ASM_SOURCES = stm/startup_stm32h743xx.s

OBJECTS += $(addprefix $(BUILD_DIR)/, $(C_SOURCES:.c=.o))
OBJECTS += $(addprefix $(BUILD_DIR)/, $(ASM_SOURCES:.s=.o))

OBJECT_DIRS = $(sort $(dir $(OBJECTS)))
$(foreach dir, $(OBJECT_DIRS),$(shell mkdir -p $(dir)))

all: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).hex $(BUILD_DIR)/$(TARGET).bin

$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR)
	@echo "\tCC\t" $<
	@$(CC) -c $(CFLAGS) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.c=.lst)) $< -o $@

$(BUILD_DIR)/%.o: %.s Makefile | $(BUILD_DIR)
	@echo "\tASM\t" $<
	@$(AS) -c $(CFLAGS) $< -o $@

$(BUILD_DIR)/$(TARGET).elf: build/assets/build/assets_icons.o $(HEADERS) $(OBJECTS) Makefile
	@echo "\tLD\t" $@
	@$(CC) $(OBJECTS) $(LDFLAGS) -o $@
	@$(SZ) $@

$(BUILD_DIR)/%.hex: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	@echo "\tHEX\t" $@
	@$(HEX) $< $@

$(BUILD_DIR)/%.bin: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	@echo "\tBIN\t" $@
	@$(BIN) $< $@

$(ASSETS_BUILD_DIR)/assets_icons.c: $(ASSETS_SOURCES) $(ASSETS_COMPILLER)
	@echo "\tASSETS\t" $@
	@mkdir -p $(ASSETS_BUILD_DIR)
	@$(ASSETS_COMPILLER) $(ASSETS_SOURCE_DIR) $(ASSETS_BUILD_DIR)

$(BUILD_DIR):
	mkdir $@

$(ASSETS_BUILD_DIR):
	mkdir $@

.PHONY: lint
lint:
	find . -type f \( -name "*.c" -o -name "*.h" \) \
		\( -path "./src/*" -o -path "./src/*" \) \
        | xargs clang-format --Werror --style=file -i --dry-run

.PHONY: format
format:
	find . -type f \( -name "*.c" -o -name "*.h" \) \
		\( -path "./src/*" -o -path "./src/*" \) \
        | xargs clang-format --Werror --style=file -i

.PHONY: flash
flash: $(BUILD_DIR)/$(TARGET).bin
	@echo "\tFLASH\t" $(BUILD_DIR)/$(TARGET).bin
	@st-flash write $(BUILD_DIR)/$(TARGET).bin 0x8000000

.PHONY: flash-dfu
flash-dfu: $(BUILD_DIR)/$(TARGET).bin
	dfu-util -a 0 -D $(BUILD_DIR)/$(TARGET).bin -s 0x08000000

.PHONY: clean
clean:
	-rm -rf ${BUILD_DIR}
	-rm -rf ${ASSETS_BUILD_DIR}
