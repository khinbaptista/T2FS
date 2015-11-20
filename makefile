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
#
########################################################################

LIB = libt2fs.a

#################################################

CC = gcc
CFLAGS = -Wall -std=c99

#################################################

LIB_DIR = ./lib
INC_DIR = ./include
BIN_DIR = ./bin
SRC_DIR = ./src

#################################################

SRC = t2fs.c

OBJ = $(SRC:.c=.o)
OBJECTS = $(patsubst %, $(BIN_DIR)/%, $(OBJ))
OBJECTS += ./lib/apidisk.o

#################################################

all: $(LIB_DIR)/$(LIB)

$(LIB_DIR)/$(LIB): $(OBJECTS)
	ar crs $@ $^
	
$(BIN_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) -c -o $@ $^ -I$(INC_DIR) $(CFLAGS)

clean:
	rm -rf $(LIB_DIR)/*.a $(BIN_DIR)/*.o $(SRC_DIR)/*~ $(INC_DIR)/*~ *~
	
ex: $(LIB_DIR)/$(LIB)
	cd ./exemplos && make clean && make
	
ex_clean:
	cd ./exemplos && make clean
