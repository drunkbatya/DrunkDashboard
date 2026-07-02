$(BUILD_DIR):
	@mkdir -p $@

$(ASSETS_TARGET): $(IMAGES) $(ASSETS_COMPILER) $(MAKE_FILES) | $(BUILD_DIR)
	@echo "\tASSETS\t" $@
	@mkdir -p $(ASSETS_BUILD_DIR)
	@$(PY) $(ASSETS_COMPILER) $(ASSETS_SRC_DIR) $(ASSETS_BUILD_DIR)

$(filter-out $(ASSETS_OBJECT),$(C_OBJECTS)): $(ASSETS_TARGET)

$(BUILD_DIR)/%.o: %.c $(MAKE_FILES) | $(BUILD_DIR)
	@echo "\tCC\t" $<
	@mkdir -p $(@D)
	@$(CC) -c $(CFLAGS) $< -o $@

$(BUILD_DIR)/%.o: %.s $(MAKE_FILES) | $(BUILD_DIR)
	@echo "\tASM\t" $<
	@mkdir -p $(@D)
	@$(AS) -c $(ASFLAGS) $< -o $@

$(BUILD_DIR)/$(TARGET).elf: $(ASSETS_TARGET) $(OBJECTS) $(MAKE_FILES) | $(BUILD_DIR)
	@echo "\tLD\t" $@
	@$(CC) $(OBJECTS) $(LDFLAGS) -o $@
	@$(SZ) $@

$(BUILD_DIR)/%.hex: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	@echo "\tHEX\t" $@
	@$(HEX) $< $@

$(BUILD_DIR)/%.bin: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	@echo "\tBIN\t" $@
	@$(BIN) $< $@

-include $(OBJECTS:.o=.d)
