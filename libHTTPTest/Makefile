GCC ?= gcc
CC = $(GCC)
LIBS = $(shell pkg-config --libs libHTTP)
CFLAGS_EXTRA = $(shell pkg-config --cflags libHTTP)
CFLAGS = -c $(CFLAGS_EXTRA) -o $*.o
LFLAGS = $(O) $(LIBS) -o $(EXE)
CHMOD = chmod 0755
INSTALL = install -m755
INSTALL_DIR = $(INSTALL) -d

O = main.o getch.o
EXE = libHTTPTest.out

default: all

all: $(EXE)

libHTTPTest.out: $(O)
	$(CC) $(LFLAGS)
	$(CHMOD) $(EXE)

install: all
	$(INSTALL_DIR) ../bin
	$(INSTALL) $(EXE) ../bin/libHTTPTest

.c.o:
	$(CC) $(CFLAGS) $*.c

clean:
	rm -f *.o *.out *~

.PHONY: clean .c.o install all default

main.o: main.c
getch.o: getch.c
