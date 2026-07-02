PREFIX ?= arm-none-eabi-

ifdef GCC_PATH
TOOLCHAIN_BIN := $(GCC_PATH)/$(PREFIX)
else
TOOLCHAIN_BIN := $(PREFIX)
endif

CC := $(TOOLCHAIN_BIN)gcc
AS := $(TOOLCHAIN_BIN)gcc -x assembler-with-cpp
CP := $(TOOLCHAIN_BIN)objcopy
SZ := $(TOOLCHAIN_BIN)size

HEX := $(CP) -O ihex
BIN := $(CP) -O binary -S
PY ?= python3

