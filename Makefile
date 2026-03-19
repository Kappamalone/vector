# Makefile

# --- Configuration ---
CXX      := g++
CXXFLAGS := -std=c++2a -O2 -Wall -Wextra -pedantic
SRC      := main.cpp
BIN_DIR  := build
TARGET   := $(BIN_DIR)/main

# --- Phony targets ---
.PHONY: all run clean

# Default: build the binary
all: $(TARGET)

# Build rule
$(TARGET): $(SRC) | $(BIN_DIR)
    $(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

# Ensure build directory exists
$(BIN_DIR):
    mkdir -p $(BIN_DIR)

# Run the compiled program
run: $(TARGET)
    ./$(TARGET)

# Clean build artifacts
clean:
    rm -rf $(BIN_DIR)
