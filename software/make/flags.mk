CPU := -mcpu=cortex-m33
FPU := -mfpu=fpv5-sp-d16
FLOAT_ABI := -mfloat-abi=hard
MCU := $(CPU) -mthumb $(FPU) $(FLOAT_ABI)

C_DEFS := \
	-DUSE_FULL_LL_DRIVER \
	-DSTM32U585xx \
	-DHSE_VALUE=25000000 \
	-DHSE_STARTUP_TIMEOUT=100 \
	-DLSE_STARTUP_TIMEOUT=5000 \
	-DLSE_VALUE=32768 \
	-DMSI_VALUE=4000000 \
	-DEXTERNALSAI1_CLOCK_VALUE=48000 \
	-DHSI_VALUE=16000000 \
	-DLSI_VALUE=32000 \
	-DVDD_VALUE=3300

C_DEFS += \
	'-DGIT_COMMIT="$(GIT_COMMIT)"' \
	'-DGIT_BRANCH="$(GIT_BRANCH)"' \
	'-DGIT_BRANCH_NUM="$(GIT_BRANCH_NUM)"' \
	'-DBUILD_DATE="$(BUILD_DATE)"' \
	'-DVERSION="$(VERSION)"' \
	'-DBUILD_DIRTY=$(GIT_DIRTY)' \
	'-DFIRMWARE_ORIGIN="DrunkDashboard"' \
	'-DGIT_ORIGIN="$(GIT_ORIGIN)"' \
	-DTARGET=$(HW_TARGET_ID)

ifeq ($(DEBUG),1)
C_DEFS += -DDEBUG -DFURI_DEBUG -DFURI_HAL_DEBUG
endif

C_INCLUDES := \
	-Isrc/lib/cmsis_wrappers/Include \
	-I$(ASSETS_BUILD_DIR) \
	-Isrc/targets/furi_hal_include \
	-Isrc/targets/$(HW_TARGET)/inc \
	-Isrc/targets/$(HW_TARGET)/furi_hal \
	-Isrc/furi \
	-Isrc/ll_wrappers \
	-Isrc/lib/FreeRTOS-glue \
	-Isrc/lib/stm32u5xx-hal-driver/Inc \
	-Isrc/lib/st_cmsis_device_u5/Include \
	-Isrc/lib \
	-Isrc/lib/toolbox \
	-Isrc/lib/datetime \
	-Isrc/lib/mlib \
	-Isrc/lib/u8g2 \
	-Isrc \
	-Isrc/applications/services \
	-Isrc/lib/FreeRTOS-Kernel/include \
	-Isrc/lib/FreeRTOS-Kernel/portable/GCC/ARM_CM33_NTZ/non_secure

CFLAGS += $(MCU) $(C_DEFS) $(C_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections

ifeq ($(DEBUG),1)
CFLAGS += -g -gdwarf-2
endif

CFLAGS += -MMD -MP -MF"$(@:%.o=%.d)"

LIBS := -lc -lm -lnosys
LIBDIR :=
LDFLAGS := $(MCU) -specs=nano.specs -T$(LDSCRIPT) $(LIBDIR) $(LIBS) \
	-Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref \
	-Wl,--gc-sections
LDFLAGS += -Wl,--wrap,_malloc_r -Wl,--wrap,_free_r -Wl,--wrap,_calloc_r -Wl,--wrap,_realloc_r
