# Makefile for ls-v2.0.0 — Feature-3 (column "down then across")

CC = gcc
CFLAGS = -Wall -g

SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

SRC = $(SRC_DIR)/ls-v1.2.0.c
OBJ = $(OBJ_DIR)/ls-v1.2.0.o
BIN = $(BIN_DIR)/ls

# Default target
all: $(BIN)

# Rule to link object file into binary
$(BIN): $(OBJ) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $(BIN) $(OBJ)

# Rule to compile .c into .o (object)
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Create directories if not exist
$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Clean up
clean:
	rm -f $(OBJ) $(BIN)
