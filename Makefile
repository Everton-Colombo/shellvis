# Makefile for shellvis project with modified structure

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -g
LDFLAGS =

# Project name
PROJECT = shellvis

# Directories
SRC_DIR = src
CORE_DIR = $(SRC_DIR)/core
EXT_CMD_DIR = $(SRC_DIR)/external_commands
OBJ_DIR = obj
BIN_DIR = bin
EXT_BIN_DIR = $(BIN_DIR)/external_commands

# Find source files
CORE_SRCS = $(wildcard $(CORE_DIR)/*.c)
EXT_CMD_SRCS = $(wildcard $(EXT_CMD_DIR)/*.c)

# Generate core object file paths
CORE_OBJS = $(patsubst $(CORE_DIR)/%.c,$(OBJ_DIR)/core/%.o,$(CORE_SRCS))

# Generate external command object file paths
EXT_CMD_OBJS = $(patsubst $(EXT_CMD_DIR)/%.c,$(OBJ_DIR)/external_commands/%.o,$(EXT_CMD_SRCS))

# Generate external command binary paths
EXT_CMD_BINS = $(patsubst $(EXT_CMD_DIR)/%.c,$(EXT_BIN_DIR)/%,$(EXT_CMD_SRCS))

# Main executable
TARGET = $(BIN_DIR)/$(PROJECT)

# Default target
all: directories $(TARGET) $(EXT_CMD_BINS)

# Create necessary directories
directories:
	@mkdir -p $(BIN_DIR)
	@mkdir -p $(EXT_BIN_DIR)
	@mkdir -p $(OBJ_DIR)/core
	@mkdir -p $(OBJ_DIR)/external_commands

# Compile core .c files to .o files
$(OBJ_DIR)/core/%.o: $(CORE_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Compile external command .c files to .o files
$(OBJ_DIR)/external_commands/%.o: $(EXT_CMD_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Link core object files into the main executable
$(TARGET): $(CORE_OBJS)
	$(CC) $(CORE_OBJS) -o $(TARGET) $(LDFLAGS)
	@echo "Build complete: $(TARGET)"

# Build each external command as a separate executable
$(EXT_BIN_DIR)/%: $(OBJ_DIR)/external_commands/%.o
	$(CC) $< -o $@ $(LDFLAGS)
	@echo "Built external command: $@"

# Run the executable
run: all
	$(TARGET)

# Clean up build files
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)
	@echo "Clean complete"

.PHONY: all directories run clean