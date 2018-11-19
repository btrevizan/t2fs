CC=gcc
LIB_DIR=../lib/
INC_DIR=../include/
BIN_DIR=../bin/
SRC_DIR=../src/

EXECUTABLES= create2_test delete2_test open2_test close2_test read2_test \
             write2_test truncate2_test seek2_test mkdir2_test rmdir2_test \
						 chdir2_test getcwd2_test opendir2_test readdir2_test \
						 closedir2_test ln2_test

all: $(EXECUTABLES)

teste.o: teste.c teste.h
	$(CC) -std=c99 -c teste.c -Wall

create2_test: create2_test.c teste.o teste.h $(INC_DIR)t2fs.h \
              $(LIB_DIR)libt2fs.a
	$(CC) -std=c99 -o create2_test create2_test.c teste.o -L$(LIB_DIR) -lt2fs \
	 -Wall -lm

delete2_test: delete2_test.c teste.o teste.h $(INC_DIR)t2fs.h \
              $(LIB_DIR)libt2fs.a
	$(CC) -std=c99 -o delete2_test delete2_test.c teste.o -L$(LIB_DIR) -lt2fs \
	 -Wall -lm

open2_test: open2_test.c teste.o teste.h $(INC_DIR)t2fs.h \
              $(LIB_DIR)libt2fs.a
	$(CC) -std=c99 -o open2_test open2_test.c teste.o -L$(LIB_DIR) -lt2fs \
	 -Wall -lm

close2_test: close2_test.c teste.o teste.h $(INC_DIR)t2fs.h \
              $(LIB_DIR)libt2fs.a
	$(CC) -std=c99 -o close2_test close2_test.c teste.o -L$(LIB_DIR) -lt2fs \
	 -Wall -lm

read2_test: read2_test.c teste.o teste.h $(INC_DIR)t2fs.h \
              $(LIB_DIR)libt2fs.a
	$(CC) -std=c99 -o read2_test read2_test.c teste.o -L$(LIB_DIR) -lt2fs \
	 -Wall -lm

write2_test: write2_test.c teste.o teste.h $(INC_DIR)t2fs.h \
              $(LIB_DIR)libt2fs.a
	$(CC) -std=c99 -o write2_test write2_test.c teste.o -L$(LIB_DIR) -lt2fs \
	 -Wall -lm

truncate2_test: truncate2_test.c teste.o teste.h $(INC_DIR)t2fs.h \
              $(LIB_DIR)libt2fs.a
	$(CC) -std=c99 -o truncate2_test truncate2_test.c teste.o -L$(LIB_DIR) \
	-lt2fs -Wall -lm

seek2_test: seek2_test.c teste.o teste.h $(INC_DIR)t2fs.h \
              $(LIB_DIR)libt2fs.a
	$(CC) -std=c99 -o seek2_test seek2_test.c teste.o -L$(LIB_DIR) -lt2fs \
	 -Wall -lm

mkdir2_test: mkdir2_test.c teste.o teste.h $(INC_DIR)t2fs.h \
              $(LIB_DIR)libt2fs.a
	$(CC) -std=c99 -o mkdir2_test mkdir2_test.c teste.o -L$(LIB_DIR) -lt2fs \
	 -Wall -lm

rmdir2_test: rmdir2_test.c teste.o teste.h $(INC_DIR)t2fs.h \
              $(LIB_DIR)libt2fs.a
	$(CC) -std=c99 -o rmdir2_test rmdir2_test.c teste.o -L$(LIB_DIR) -lt2fs \
	 -Wall -lm

chdir2_test: chdir2_test.c teste.o teste.h $(INC_DIR)t2fs.h \
              $(LIB_DIR)libt2fs.a
	$(CC) -std=c99 -o chdir2_test chdir2_test.c teste.o -L$(LIB_DIR) -lt2fs \
	 -Wall -lm

getcwd2_test: getcwd2_test.c teste.o teste.h $(INC_DIR)t2fs.h \
              $(LIB_DIR)libt2fs.a
	$(CC) -std=c99 -o getcwd2_test getcwd2_test.c teste.o -L$(LIB_DIR) -lt2fs \
	 -Wall -lm

opendir2_test: opendir2_test.c teste.o teste.h $(INC_DIR)t2fs.h \
              $(LIB_DIR)libt2fs.a
	$(CC) -std=c99 -o opendir2_test opendir2_test.c teste.o -L$(LIB_DIR) -lt2fs \
	 -Wall -lm

readdir2_test: readdir2_test.c teste.o teste.h $(INC_DIR)t2fs.h \
              $(LIB_DIR)libt2fs.a
	$(CC) -std=c99 -o readdir2_test readdir2_test.c teste.o -L$(LIB_DIR) -lt2fs \
	 -Wall -lm

closedir2_test: closedir2_test.c teste.o teste.h $(INC_DIR)t2fs.h \
              $(LIB_DIR)libt2fs.a
	$(CC) -std=c99 -o closedir2_test closedir2_test.c teste.o -L$(LIB_DIR) \
	 -lt2fs -Wall -lm

ln2_test: ln2_test.c teste.o teste.h $(INC_DIR)t2fs.h \
              $(LIB_DIR)libt2fs.a
	$(CC) -std=c99 -o ln2_test ln2_test.c teste.o -L$(LIB_DIR) \
	 -lt2fs -Wall -lm

run: all
	for exec in $(EXECUTABLES); do cp -a t2fs_disk_template.dat t2fs_disk.dat; ./$$exec; done

clean:
	rm -rf $(EXECUTABLES) teste.o