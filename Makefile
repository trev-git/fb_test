CC=gcc
CFLAGS=-W -Wall -Wextra -g
SRC=src
OBJ=obj
BIN=bin

OBJS=$(OBJ)/main.o \
     $(OBJ)/bmp.o

all: mk $(BIN)/fb_test

$(BIN)/fb_test: $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

$(OBJ)/%.o: $(SRC)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ) $(BIN)

mk:
	mkdir -p $(BIN) $(OBJ)

.PHONY: all clean mk
