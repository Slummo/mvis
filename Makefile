BUILD_DIR	:= build
LOG_LEVEL	:= ERROR

all: $(BUILD_DIR)/CMakeCache.txt
	@cmake --build $(BUILD_DIR) -- -s

$(BUILD_DIR)/CMakeCache.txt:
	@mkdir -p $(BUILD_DIR)
	@cmake -S . -B $(BUILD_DIR) --log-level=$(LOG_LEVEL)

run: all
	@$(BUILD_DIR)/main $(ARGS)

clean:
	@rm -rf $(BUILD_DIR)

.PHONY: all run clean