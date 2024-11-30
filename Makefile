# Variables
BUILD_DIR := build
CMAKE := cmake
EXECUTABLE_QR := $(BUILD_DIR)/qr
EXECUTABLE_TEST := $(BUILD_DIR)/qr_test
CTEST := ctest

# Targets
.PHONY: all build clean test run run-qr

# Default target
all: build

# Build the project using CMake
build:
	@$(CMAKE) -S . -B $(BUILD_DIR)
	@$(CMAKE) --build $(BUILD_DIR)

# Clean the build directory
clean:
	@$(CMAKE) --build $(BUILD_DIR) --target clean || true
	@rm -rf $(BUILD_DIR)

# Run tests using CTest
test: build
	@$(CTEST) --test-dir $(BUILD_DIR)

# Run the qr_test executable
run:
	@$(EXECUTABLE_TEST)

# Build and run the main qr executable
run-qr: build
	@$(EXECUTABLE_QR)

fmt:
	clang-format -i *.cc *.h
