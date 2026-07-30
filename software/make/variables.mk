TARGET ?= DrunkDashboard
HW_TARGET ?= d2
HW_TARGET_ID ?= $(subst d,,$(HW_TARGET))
DEBUG ?= 1
OPT ?= -Og
BUILD_ROOT ?= build
BUILD_DIR ?= $(BUILD_ROOT)/$(HW_TARGET)
JOBS ?= $(shell nproc 2>/dev/null || echo 1)

ifeq ($(filter -j% --jobs%,$(MAKEFLAGS)),)
MAKEFLAGS += -j$(JOBS)
endif

ASSETS_SRC_DIR ?= assets
ASSETS_BUILD_DIR ?= $(BUILD_DIR)/assets
ASSETS_TARGET ?= $(ASSETS_BUILD_DIR)/assets_icons.c
ASSETS_COMPILER ?= scripts/icon_convert.py

OPENOCD_CFG ?= -f interface/stlink.cfg -f target/stm32u5x.cfg

TARGET_DIR := src/targets/$(HW_TARGET)
TARGET_MK := $(TARGET_DIR)/target.mk

ifeq ($(wildcard $(TARGET_MK)),)
$(error Unknown HW_TARGET '$(HW_TARGET)': $(TARGET_MK) not found)
endif

include $(TARGET_MK)

MAKE_FILES := $(shell find make -type f -name '*.mk') $(TARGET_MK)
