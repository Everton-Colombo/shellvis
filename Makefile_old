# Makefile for shellvis project

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra
LDFLAGS =

# Project name
PROJECT = shellvis

# Directories
SRC_DIR = .
OBJ_DIR = obj
BIN_DIR = bin

# Find all C source files recursively
SRCS = $(shell find $(SRC_DIR) -name "*.c")
# Generate object file paths
OBJS = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))
# Main executable
TARGET = $(BIN_DIR)/$(PROJECT)

# Default target
all: directories $(TARGET)

# Create necessary directories
directories:
	@mkdir -p $(BIN_DIR)
	@mkdir -p $(dir $(OBJS))

# Compile .c files to .o files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Link object files into the executable
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)
	@echo "Build complete: $(TARGET)"

# Run the executable
run: all
	$(TARGET)

# Clean up build files
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)
	@echo "Clean complete"

.PHONY: all directories run clean