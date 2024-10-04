# Compiler and flags
CC = gcc
CFLAGS = -Wall -pthread -Iinclude

# Directories
SRC_DIR = src
INCLUDE_DIR = include
OBJ_DIR = obj
BIN_DIR = bin

# Target executable
TARGET = $(BIN_DIR)/scheduling_ships

# Source files
SRCS = $(SRC_DIR)/main.c $(SRC_DIR)/CEthreads.c \
       $(SRC_DIR)/ship.c

# Object files (replace .c with .o and store in OBJ_DIR)
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# Rule to build the target executable
$(TARGET): $(OBJS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# Rule to build object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean the project (remove object files and binary)
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

# Run the project
run: $(TARGET)
	./$(TARGET)

# Rule to build with optional hardware support
hardware: CFLAGS += -DHARDWARE_SUPPORT
hardware: $(TARGET)

.PHONY: clean run hardware
