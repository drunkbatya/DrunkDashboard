C_SOURCES := \
	$(ASSETS_TARGET) \
	src/lib/stm32u5xx-hal-driver/Src/stm32u5xx_ll_fdcan.c \
	src/lib/stm32u5xx-hal-driver/Src/stm32u5xx_ll_adc.c \
	src/lib/stm32u5xx-hal-driver/Src/stm32u5xx_ll_dma.c \
	src/lib/stm32u5xx-hal-driver/Src/stm32u5xx_ll_exti.c \
	src/lib/stm32u5xx-hal-driver/Src/stm32u5xx_ll_gpio.c \
	src/lib/stm32u5xx-hal-driver/Src/stm32u5xx_ll_i2c.c \
	src/lib/stm32u5xx-hal-driver/Src/stm32u5xx_ll_lptim.c \
	src/lib/stm32u5xx-hal-driver/Src/stm32u5xx_ll_pwr.c \
	src/lib/stm32u5xx-hal-driver/Src/stm32u5xx_ll_rcc.c \
	src/lib/stm32u5xx-hal-driver/Src/stm32u5xx_ll_rtc.c \
	src/lib/stm32u5xx-hal-driver/Src/stm32u5xx_ll_spi.c \
	src/lib/stm32u5xx-hal-driver/Src/stm32u5xx_ll_tim.c \
	src/lib/stm32u5xx-hal-driver/Src/stm32u5xx_ll_usart.c \
	src/lib/stm32u5xx-hal-driver/Src/stm32u5xx_ll_utils.c \
	src/lib/FreeRTOS-Kernel/event_groups.c \
	src/lib/FreeRTOS-Kernel/list.c \
	src/lib/FreeRTOS-Kernel/queue.c \
	src/lib/FreeRTOS-Kernel/stream_buffer.c \
	src/lib/FreeRTOS-Kernel/tasks.c \
	src/lib/FreeRTOS-Kernel/timers.c \
	src/lib/FreeRTOS-Kernel/portable/GCC/ARM_CM33_NTZ/non_secure/port.c \
	src/lib/FreeRTOS-Kernel/portable/GCC/ARM_CM33_NTZ/non_secure/portasm.c

C_SOURCES += $(shell find src \
	-type d -name '.*' -prune -o \
	-path 'src/targets' -prune -o \
	-path 'src/lib/FreeRTOS-Kernel' -prune -o \
	-path 'src/lib/mlib' -prune -o \
	-path 'src/lib/stm32u5xx-hal-driver' -prune -o \
	-path 'src/lib/cmsis_wrappers' -prune -o \
	-type f -name '*.c' -print)

layer_sources = $(shell find $(1) -type f -name '*.c')
TARGET_ALL_SOURCES := $(foreach layer,$(TARGET_LAYERS),$(call layer_sources,$(layer)))
TARGET_SOURCE_NAMES := $(filter-out $(EXCLUDED_SOURCES),$(sort $(notdir $(TARGET_ALL_SOURCES))))
TARGET_SOURCES := \
	$(foreach name,$(TARGET_SOURCE_NAMES),$(firstword $(filter %/$(name),$(TARGET_ALL_SOURCES))))

C_SOURCES += $(TARGET_SOURCES)

C_OBJECTS := $(addprefix $(BUILD_DIR)/,$(C_SOURCES:.c=.o))
ASSETS_OBJECT := $(BUILD_DIR)/$(ASSETS_TARGET:.c=.o)
OBJECTS := $(C_OBJECTS)
OBJECT_DIRS := $(sort $(dir $(OBJECTS)))

IMAGES := $(shell find $(ASSETS_SRC_DIR) -type f -iname '*.png')
LDSCRIPT := $(firstword \
	$(foreach layer,$(TARGET_LAYERS),$(shell find $(layer) -type f -name '*FLASH.ld')))
