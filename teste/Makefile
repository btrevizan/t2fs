CC=gcc
LIB_DIR=../lib/
INC_DIR=../include/
BIN_DIR=../bin/
SRC_DIR=../src/

EXECUTABLES= create2_test delete2_test open2_test close2_test

all: $(EXECUTABLES)

teste.o: teste.c teste.h
	$(CC) -std=c99 -c teste.c -Wall

create2_test: create2_test.c teste.o teste.h $(INC_DIR)t2fs.h \
              $(LIB_DIR)libt2fs.a
	$(CC) -std=c99 -o create2_test create2_test.c teste.o -L$(LIB_DIR) -lt2fs \
	 -Wall

delete2_test: delete2_test.c teste.o teste.h $(INC_DIR)t2fs.h \
              $(LIB_DIR)libt2fs.a
	$(CC) -std=c99 -o delete2_test delete2_test.c teste.o -L$(LIB_DIR) -lt2fs \
	 -Wall

open2_test: open2_test.c teste.o teste.h $(INC_DIR)t2fs.h \
              $(LIB_DIR)libt2fs.a
	$(CC) -std=c99 -o open2_test open2_test.c teste.o -L$(LIB_DIR) -lt2fs \
	 -Wall

close2_test: close2_test.c teste.o teste.h $(INC_DIR)t2fs.h \
              $(LIB_DIR)libt2fs.a
	$(CC) -std=c99 -o close2_test close2_test.c teste.o -L$(LIB_DIR) -lt2fs \
	 -Wall

run: all
	for exec in $(EXECUTABLES); do cp -a t2fs_disk_template.dat t2fs_disk.dat; ./$$exec; done

clean:
	rm -rf $(EXECUTABLES) teste.o t2fs_disk.dat