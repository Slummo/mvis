BUILD_DEBUG   := build/debug
BUILD_RELEASE := build/release

CMAKE_ARGS    :=
CORES         ?= $(shell nproc 2>/dev/null || echo 4)
APP_NAME      := mvis

.PHONY: all debug release run clean clean-full help

all: debug

$(BUILD_DEBUG)/CMakeCache.txt: CMakeLists.txt
	@echo "Configuring Debug build..."
	@cmake -S . -B $(BUILD_DEBUG) -DCMAKE_BUILD_TYPE=Debug $(CMAKE_ARGS)

$(BUILD_RELEASE)/CMakeCache.txt: CMakeLists.txt
	@echo "Configuring Release build..."
	@cmake -S . -B $(BUILD_RELEASE) -DCMAKE_BUILD_TYPE=Release $(CMAKE_ARGS)

debug: $(BUILD_DEBUG)/CMakeCache.txt
	@echo "Building Debug..."
	@cmake --build $(BUILD_DEBUG) -j $(CORES)

release: $(BUILD_RELEASE)/CMakeCache.txt
	@echo "Building Release..."
	@cmake --build $(BUILD_RELEASE) -j $(CORES)

run: debug
	@echo "Running $(APP_NAME)..."
	@./$(BUILD_DEBUG)/bin/$(APP_NAME)

run-release: release
	@echo "Running $(APP_NAME) (Release)..."
	@./$(BUILD_RELEASE)/bin/$(APP_NAME)

clean:
	@echo "Cleaning standard build outputs..."
	@rm -rf $(BUILD_DEBUG)/test

clean-full:
	@echo "Cleaning build directory..."
	@rm -rf build

help:
	@echo "Usage:"
	@echo "  make                 - Build Debug version"
	@echo "  make debug           - Build Debug version"
	@echo "  make release         - Build Release version"
	@echo "  make run             - Build and run the debug application"
	@echo "  make run-release     - Build and run the release application"
	@echo "  make clean           - Remove local object files"
	@echo "  make clean-full      - Remove all build files"