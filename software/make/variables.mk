TARGET ?= DrunkDashboard
HW_TARGET ?= d1
HW_TARGET_ID ?= $(subst d,,$(HW_TARGET))
DEBUG ?= 1
OPT ?= -Og
BUILD_DIR ?= build
JOBS ?= $(shell nproc 2>/dev/null || echo 1)

ifeq ($(filter -j% --jobs%,$(MAKEFLAGS)),)
MAKEFLAGS += -j$(JOBS)
endif

ASSETS_SRC_DIR ?= assets
ASSETS_BUILD_DIR ?= $(BUILD_DIR)/assets
ASSETS_TARGET ?= $(ASSETS_BUILD_DIR)/assets_icons.c
ASSETS_COMPILER ?= scripts/icon_convert.py

OPENOCD_CFG ?= -f interface/stlink.cfg -f target/stm32u5x.cfg

MAKE_FILES := $(shell find make -type f -name '*.mk')
