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
CFLAGS = -Wall -std=c99

#################################################

LIB_DIR = ../lib
INC_DIR = ../include
BIN_DIR = ../bin

#################################################
# Each source must be a complete program with a test

SRC = hello.c

TARGETS = $(SRC:.c=.run)

#################################################

all: $(TARGETS)

%.run: %.c
	$(CC) -o $@ $^ $(CFLAGS) -I$(INC_DIR) -L$(LIB_DIR) -lt2fs
	./$@

clean:
	rm -rf *.run
