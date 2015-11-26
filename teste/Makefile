########################################################################
#
#	INF01142 - Operating Systems
#		Assignment 2 - File Systems
#
#	T2FS
#
#	João Lauro Garibaldi Jr		195505
#	Khin Baptista				217443
#
#	Make examples and tests
#
########################################################################

CC = gcc
CFLAGS = -Wall -std=c99 -g

#################################################

LIB_DIR = ../lib
INC_DIR = ../include
BIN_DIR = ../bin

#################################################
# Each source must be a complete program with a test

SRC = dir.c

TARGETS = $(BIN_DIR)/$(SRC:.c=.run)

#################################################

all: $(TARGETS)

$(BIN_DIR)/%.run: %.c
	$(CC) -o $@ $^ $(CFLAGS) -I$(INC_DIR) -L$(LIB_DIR) -lt2fs

run: $(TARGETS)
	./$<

clean:
	rm -rf *.run
