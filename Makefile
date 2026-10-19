CC = gcc
CFLAGS = -Wall -Wextra -Werror -g -Iinclude
BIN_DIR = bin

all: $(BIN_DIR)/pipe_lab

check: all
	./scripts/check.sh

grade: all
	./scripts/grade.sh

$(BIN_DIR)/pipe_lab: src/pipe_lab.c include/pipe_lab.h
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(BIN_DIR)/pipe_lab

.PHONY: all check grade clean