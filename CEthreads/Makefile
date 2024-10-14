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

# Source files (asegúrate de incluir schedulers.c y flow_methods.c aquí)
SRCS = $(SRC_DIR)/main.c \
       $(SRC_DIR)/ship.c \
       $(SRC_DIR)/schedulers.c $(SRC_DIR)/flow_manager.c

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

# Run the project with an argument (default scheduler type is round_robin)
run: $(TARGET)
	./$(TARGET) $(SCHEDULER_TYPE)

# Rule to build with optional hardware support
hardware: CFLAGS += -DHARDWARE_SUPPORT
hardware: $(TARGET)

.PHONY: clean run hardware
