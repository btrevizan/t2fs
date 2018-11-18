CC=gcc
LIB_DIR=./lib/
INC_DIR=./include/
BIN_DIR=./bin/
SRC_DIR=./src/

all: libt2fs.a

libt2fs.a: lib.o $(LIB_DIR)apidisk.o
	ar crs $(LIB_DIR)libt2fs.a $(BIN_DIR)lib.o $(LIB_DIR)apidisk.o

lib.o: $(SRC_DIR)lib.c $(INC_DIR)apidisk.h $(INC_DIR)t2fs.h
	$(CC) -std=c99 -c -o $(BIN_DIR)lib.o $(SRC_DIR)lib.c -Wall

clean:
	rm -rf $(LIB_DIR)/*.a $(BIN_DIR)/*.o $(SRC_DIR)/*~ $(INC_DIR)/*~ *~


