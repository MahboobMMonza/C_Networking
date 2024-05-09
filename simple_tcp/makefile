SRC=src
BIN=bin

CC=gcc
FLAGS:=-Wall
LDFLAGS=


all: $(BIN)/server $(BIN)/client

$(BIN)/%: $(SRC)/%.c
	gcc -o $@ $< $(FLAGS) $(LDFLAGS)

clean:
	rm -f $(BIN)/*
