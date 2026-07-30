all: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).hex $(BUILD_DIR)/$(TARGET).bin

.PHONY: lint
lint:
	find src \
		-type d -name '.*' -prune -o \
		-path 'src/lib/FreeRTOS-Kernel' -prune -o \
		-path 'src/lib/mlib' -prune -o \
		-path 'src/lib/u8g2' -prune -o \
		-path 'src/lib/stm32u5xx-hal-driver' -prune -o \
		-path 'src/lib/cmsis_wrappers' -prune -o \
		-type f -name '*.c' -print \
		| xargs clang-format --Werror --style=file -i --dry-run

.PHONY: format
format:
	find src \
		-type d -name '.*' -prune -o \
		-path 'src/lib/FreeRTOS-Kernel' -prune -o \
		-path 'src/lib/mlib' -prune -o \
		-path 'src/lib/u8g2' -prune -o \
		-path 'src/lib/stm32u5xx-hal-driver' -prune -o \
		-path 'src/lib/cmsis_wrappers' -prune -o \
		-type f -name '*.c' -print \
		| xargs clang-format --Werror --style=file -i

.PHONY: flash
flash: $(BUILD_DIR)/$(TARGET).bin
	@echo "\tFLASH\t" $(BUILD_DIR)/$(TARGET).bin
	@openocd -f interface/stlink.cfg -f target/stm32u5x.cfg -c init -c "reset halt" -c "flash write_image erase $(BUILD_DIR)/$(TARGET).bin 0x08000000" -c "reset" -c shutdown

.PHONY: flash-dfu
flash-dfu: $(BUILD_DIR)/$(TARGET).bin
	dfu-util -w -a 0 -D $(BUILD_DIR)/$(TARGET).bin -s 0x08000000

.PHONY: debug
debug: flash
	@openocd $(OPENOCD_CFG) \
		-c "transport select hla_swd" \
		-c "init" \
		-c "reset halt" & \
	OPENOCD_PID=$$!; \
	trap 'kill $$OPENOCD_PID 2>/dev/null; wait $$OPENOCD_PID 2>/dev/null' EXIT INT TERM; \
	sleep 1; \
	arm-none-eabi-gdb $(BUILD_DIR)/$(TARGET).elf \
		-ex "target extended-remote localhost:3333" \
		-ex "monitor reset halt" \
		-ex "continue"

.PHONY: clean
clean:
	-rm -fR $(BUILD_DIR)

.PHONY: clean-all
clean-all:
	-rm -fR $(BUILD_ROOT)

