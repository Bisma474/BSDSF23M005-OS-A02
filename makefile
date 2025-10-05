# Makefile for v1.0.0 - Basic directory listing
CC = gcc
CFLAGS = -Wall -Wextra -std=gnu11

SRC = src/ls-v1.0.0.c
BIN_DIR = bin
BIN = $(BIN_DIR)/ls-v1.0.0

all: $(BIN)

# Rule to build binary inside bin/
$(BIN): $(SRC) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $(BIN) $(SRC)

# Create bin directory if it doesn't exist
$(BIN_DIR):
	mkdir -p $(BIN_DIR)

clean:
	rm -f $(BIN)

