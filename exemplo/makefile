CC=gcc
LIB_DIR=../lib/
INC_DIR=../include/
BIN_DIR=../bin/
SRC_DIR=../src/

main: main.c $(INC_DIR)t2fs.h $(LIB_DIR)libt2fs.a t2fs_disk.dat
	$(CC) -std=c99 -o main main.c -L$(LIB_DIR) -lt2fs -Wall

run: main
	./main