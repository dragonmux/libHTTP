CC = gcc
LIBS = `pkg-config --libs libHTTP`
CFLAGS = -c `pkg-config --cflags libHTTP` -o $*.o
LFLAGS = $(O) $(LIBS) -o $(EXE)

O = main.o getch.o
EXE = libHTTPTest.out

default: all

all: $(O) $(EXE)

libHTTPTest.out:
	rm -f $(EXE)
	$(CC) $(LFLAGS)
	chmod +x $(EXE)

install: all
	if [ ! -d ../../bin ]; then \
		mkdir ../../bin; \
	fi
	cp $(EXE) ../../bin/libHTTPTest

.c.o:
	$(CC) $(CFLAGS) $*.c

clean:
	rm -f *.o *.out *~

main.o: main.c
getch.o: getch.c
